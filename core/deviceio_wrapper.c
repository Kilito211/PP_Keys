/**
 * @file deviceio_wrapper.c
 * @brief 键盘驱动层输入注入
 *        终极方案: 从 csrss.exe 偷句柄 -> DeviceIoControl/NtDeviceIoControlFile
 * @version 0.3
 * @date 2026-07-23
 */

#include <stdio.h>
#include "deviceio_wrapper.h"
#include <tlhelp32.h>

/* --- NtDeviceIoControlFile --- */
typedef LONG (NTAPI *fn_NtDeviceIoControlFile_t)(
    HANDLE, HANDLE, PVOID, PVOID, PVOID, ULONG, PVOID, ULONG, PVOID, ULONG);

/* IO_STATUS_BLOCK (DDK兼容) */
typedef struct {
    union { LONG Status; PVOID Pointer; };
    ULONG_PTR Information;
} DEVIO_IO_STATUS_BLOCK;

/* --- NT API 声明 --- */
typedef LONG NTSTATUS;

typedef struct _UNICODE_STRING { USHORT Length; USHORT MaximumLength; PWSTR Buffer; } UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length; HANDLE RootDirectory; PUNICODE_STRING ObjectName;
    ULONG Attributes; PVOID SecurityDescriptor; PVOID SecurityQoS;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

/* OBJECT_INFORMATION_CLASS for NtQueryObject */
#define ObjectNameInformation 1
#define ObjectTypeInformation 2

typedef struct _OBJECT_NAME_INFORMATION {
    UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION {
    UNICODE_STRING TypeName;
    ULONG Reserved[22];
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef NTSTATUS (NTAPI *fn_NtQuerySystemInformation_t)(
    ULONG, PVOID, ULONG, PULONG);
typedef NTSTATUS (NTAPI *fn_NtOpenProcess_t)(
    PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID);
typedef NTSTATUS (NTAPI *fn_NtDuplicateObject_t)(
    HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);
typedef NTSTATUS (NTAPI *fn_NtQueryObject_t)(
    HANDLE, ULONG, PVOID, ULONG, PULONG);
typedef NTSTATUS (NTAPI *fn_NtCreateFile_t)(
    PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PVOID, PVOID, ULONG,
    ULONG, ULONG, ULONG, PVOID, ULONG);
typedef VOID (NTAPI *fn_RtlInitUnicodeString_t)(PUNICODE_STRING, PCWSTR);

static fn_NtDeviceIoControlFile_t s_pNtDeviceIoControlFile = NULL;
static HANDLE s_hKbd = INVALID_HANDLE_VALUE;
static int s_send_method = 0;

static bool load_nt_api(void)
{
    if (s_pNtDeviceIoControlFile)
        return true;
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;
    s_pNtDeviceIoControlFile = (fn_NtDeviceIoControlFile_t)
        GetProcAddress(hNtdll, "NtDeviceIoControlFile");
    if (s_pNtDeviceIoControlFile) {
        printf("DEVIO: NtDeviceIoControlFile loaded\n"); fflush(stdout);
        return true;
    }
    return false;
}

/* --- 启用 SeDebugPrivilege --- */
static bool enable_debug_priv(void)
{
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (!LookupPrivilegeValueW(NULL, L"SeDebugPrivilege", &luid)) {
        CloseHandle(hToken); return false;
    }
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    BOOL ok = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    CloseHandle(hToken);
    return ok && GetLastError() == ERROR_SUCCESS;
}

/* --- 找到 csrss.exe 的 PID --- */
static DWORD find_csrss_pid(void)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    DWORD pid = 0;
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"csrss.exe") == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return pid;
}

bool deviceio_wrapper_init(void)
{
    if (s_hKbd != INVALID_HANDLE_VALUE)
        return true;
    load_nt_api();
    printf("DEVIO: trying low-privilege open...\n"); fflush(stdout);

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");

    /* 尝试 DefineDosDevice + 低权限打开 */
    const wchar_t *ntDevs[] = {
        L"\\Device\\KeyboardClass0",
        L"\\Device\\KeyboardClass1",
        NULL
    };

    /* 访问权限从低到高尝试: NtDeviceIoControlFile 不验证句柄权限,所以哪怕只读也能发 IOCTL */
    DWORD accessList[] = {
        FILE_READ_ATTRIBUTES | SYNCHRONIZE,  /* 0x00100080 最小权限 */
        FILE_READ_ATTRIBUTES,                /* 0x00000080 */
        0,                                    /* 0 = 仅查询 */
        GENERIC_READ,                         /* 兜底 */
        GENERIC_READ | GENERIC_WRITE,        /* 标准 */
    };

    wchar_t sym[] = L"PKbd";
    for (int n = 0; ntDevs[n]; n++)
    {
        if (s_hKbd != INVALID_HANDLE_VALUE) break;
        DefineDosDeviceW(DDD_REMOVE_DEFINITION, sym, NULL);
        if (!DefineDosDeviceW(DDD_RAW_TARGET_PATH, sym, ntDevs[n])) {
            printf("DEVIO: symlink fail for %ls (err=%lu)\n", ntDevs[n], GetLastError()); fflush(stdout);
            continue;
        }

        wchar_t dos[64];
        swprintf(dos, 64, L"\\\\.\\%ls", sym);

        for (int a = 0; a < 5; a++)
        {
            s_hKbd = CreateFileW(dos, accessList[a],
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                NULL, OPEN_EXISTING, 0, NULL);
            if (s_hKbd != INVALID_HANDLE_VALUE)
            {
                printf("DEVIO: opened %ls access=0x%08lX\n", dos, accessList[a]); fflush(stdout);
                /* 测试发一个空 IOCTL 看是否报错 */
                DEVIO_KEYBOARD_INPUT_DATA kid;
                ZeroMemory(&kid, sizeof(kid));
                kid.UnitId = 0; kid.MakeCode = 0; kid.Flags = 0;
                DEVIO_IO_STATUS_BLOCK iosb = {{0}, 0};
                if (s_pNtDeviceIoControlFile) {
                    LONG s = s_pNtDeviceIoControlFile(s_hKbd, NULL, NULL, NULL, &iosb,
                        DEVIO_IOCTL_INSERT_SCANCODE, &kid, sizeof(kid), NULL, 0);
                    printf("DEVIO: test IOCTL via NT -> status=0x%08lX iosb=0x%08lX\n", s, (ULONG)iosb.Status); fflush(stdout);
                }
                return true;
            }
            DWORD err = GetLastError();
            if (a == 0) {
                printf("DEVIO: %ls access=0x%08lX -> err=%lu\n", dos, accessList[a], err); fflush(stdout);
            }
        }
        DefineDosDeviceW(DDD_REMOVE_DEFINITION, sym, NULL);
    }

    printf("DEVIO: all open attempts failed\n"); fflush(stdout);
    return false;
}

/* --- fill_kid, send_via_nt, send_via_ioctl, send_stroke (unchanged from v0.2) --- */

static void fill_kid(DEVIO_KEYBOARD_INPUT_DATA *kid, uint16_t vk, uint16_t flags)
{
    UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    ZeroMemory(kid, sizeof(*kid));
    kid->UnitId = 0;
    kid->MakeCode = (USHORT)(scan & 0x00FF);
    kid->Flags = flags;
    kid->Reserved = 0;
    kid->ExtraInformation = 0xA1B2C3D4;
    if ((scan >> 8) == 0xE0)
        kid->Flags |= DEVIO_KEY_E0;
}

static bool send_via_nt(DEVIO_KEYBOARD_INPUT_DATA *kid)
{
    if (!s_pNtDeviceIoControlFile) return false;
    DEVIO_IO_STATUS_BLOCK iosb = {{0}, 0};
    LONG status = s_pNtDeviceIoControlFile(
        s_hKbd, NULL, NULL, NULL, &iosb,
        DEVIO_IOCTL_INSERT_SCANCODE,
        kid, sizeof(*kid), NULL, 0);
    if (status < 0 || iosb.Status < 0) {
        printf("DEVIO: NT send failed (status=0x%08lX, iosb=0x%08lX)\n",
               status, iosb.Status); fflush(stdout);
        return false;
    }
    return true;
}

static bool send_via_ioctl(DEVIO_KEYBOARD_INPUT_DATA *kid)
{
    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(s_hKbd, DEVIO_IOCTL_INSERT_SCANCODE,
        kid, sizeof(*kid), NULL, 0, &bytesReturned, NULL);
    if (!ok) {
        printf("DEVIO: IOCTL send failed (err=%lu)\n", GetLastError()); fflush(stdout);
    }
    return ok ? true : false;
}

static bool send_stroke(uint16_t vk, uint16_t flags)
{
    if (s_hKbd == INVALID_HANDLE_VALUE)
        if (!deviceio_wrapper_init())
            return false;

    DEVIO_KEYBOARD_INPUT_DATA kid;
    fill_kid(&kid, vk, flags);

    if (s_pNtDeviceIoControlFile && s_send_method != 2) {
        if (send_via_nt(&kid)) {
            if (s_send_method == 0) {
                s_send_method = 1;
                printf("DEVIO: using NtDeviceIoControlFile path\n"); fflush(stdout);
            }
            return true;
        }
        if (s_send_method == 0)
            printf("DEVIO: NT path failed, trying DeviceIoControl\n"); fflush(stdout);
    }

    if (send_via_ioctl(&kid)) {
        if (s_send_method == 0) {
            s_send_method = 2;
            printf("DEVIO: using DeviceIoControl path\n"); fflush(stdout);
        }
        return true;
    }
    return false;
}

/* --- 公共接口 --- */
bool deviceio_wrapper_key_down(uint16_t vk) { return send_stroke(vk, DEVIO_KEY_MAKE); }
bool deviceio_wrapper_key_up(uint16_t vk)   { return send_stroke(vk, DEVIO_KEY_BREAK); }

bool deviceio_wrapper_send_batch(const DEVIO_STROKE_BATCH *batch)
{
    if (!batch || batch->count == 0 || batch->count > DEVIO_MAX_STROKES) return false;
    if (s_hKbd == INVALID_HANDLE_VALUE)
        if (!deviceio_wrapper_init()) return false;

    DWORD inSize = batch->count * sizeof(DEVIO_KEYBOARD_INPUT_DATA);
    DWORD bytesReturned = 0;
    if (s_pNtDeviceIoControlFile) {
        DEVIO_IO_STATUS_BLOCK iosb = {{0}, 0};
        LONG st = s_pNtDeviceIoControlFile(s_hKbd, NULL, NULL, NULL, &iosb,
            DEVIO_IOCTL_INSERT_SCANCODE, (PVOID)batch->strokes, inSize, NULL, 0);
        if (st >= 0 && iosb.Status >= 0) return true;
    }
    BOOL ok = DeviceIoControl(s_hKbd, DEVIO_IOCTL_INSERT_SCANCODE,
        (PVOID)batch->strokes, inSize, NULL, 0, &bytesReturned, NULL);
    if (!ok) printf("DEVIO: batch send failed (err=%lu)\n", GetLastError()); fflush(stdout);
    return ok ? true : false;
}

void deviceio_wrapper_deinit(void)
{
    if (s_hKbd != INVALID_HANDLE_VALUE) {
        CloseHandle(s_hKbd);
        s_hKbd = INVALID_HANDLE_VALUE;
    }
    s_send_method = 0;
}
