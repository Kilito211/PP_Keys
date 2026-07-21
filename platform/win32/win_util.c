#include "win_util.h"
#include <stdbool.h>
#include <windows.h>
#include <shellapi.h>

bool is_admin(void)
{
    BOOL fIsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    if (pAdministratorsGroup != NULL)
    {
        FreeSid(pAdministratorsGroup);
    }

    return fIsAdmin == TRUE;
}

bool run_as_admin(void)
{
    WCHAR szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath)) == 0)
    {
        return false;
    }

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == NULL)
    {
        return false;
    }

    WCHAR szParams[MAX_PATH] = {0};
    for (int i = 1; i < argc; i++)
    {
        if (i > 1)
        {
            wcscat_s(szParams, ARRAYSIZE(szParams), L" ");
        }
        bool hasSpace = wcsstr(argv[i], L" ") != NULL;
        if (hasSpace)
        {
            wcscat_s(szParams, ARRAYSIZE(szParams), L"\"");
        }
        wcscat_s(szParams, ARRAYSIZE(szParams), argv[i]);
        if (hasSpace)
        {
            wcscat_s(szParams, ARRAYSIZE(szParams), L"\"");
        }
    }
    LocalFree(argv);

    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = NULL;
    sei.lpVerb = L"runas";
    sei.lpFile = szPath;
    sei.lpParameters = szParams[0] != L'\0' ? szParams : NULL;
    sei.lpDirectory = NULL;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei))
    {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_CANCELLED)
        {
            return false;
        }
        return false;
    }

    if (sei.hProcess != NULL)
    {
        CloseHandle(sei.hProcess);
    }

    return true;
}
