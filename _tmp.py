with open('core/keyboard.c', 'r', encoding='utf-8') as f:
    t = f.read()

t = t.replace('#include "keyboard.h"', '#include "keyboard.h"\n#include "interception_wrapper.h"')
t = t.replace('case INPUT_NT_SENDINPUT:       return L"NtUserSendInput";', 'case INPUT_NT_SENDINPUT:       return L"NtUserSendInput";\n    case INPUT_INTERCEPTION:       return L"Interception";')
t = t.replace('mode <= INPUT_NT_SENDINPUT)', 'mode <= INPUT_INTERCEPTION)')
t = t.replace('bool keyboard_init(void) { return true; }', 'bool keyboard_init(void) { interception_wrapper_init(); return true; }')
t = t.replace('void keyboard_deinit(void)\n{\n    interception_wrapper_deinit();\n    s_NtUserSendInput = NULL;\n}', 'WRONG')
t = t.replace('WRONG', 'void keyboard_deinit(void)\n{\n    interception_wrapper_deinit();\n    s_NtUserSendInput = NULL;\n}')

# Insert interception case before default
t = t.replace('    default:\n        return false;\n}\n\nbool keyboard_key_down', '    case INPUT_INTERCEPTION:\n    {\n        if (!interception_wrapper_key_down(key)) return false;\n        Sleep(50);\n        return interception_wrapper_key_up(key);\n    }\n    default:\n        return false;\n}\n\nbool keyboard_key_down')

with open('core/keyboard.c', 'w', encoding='utf-8') as f:
    f.write(t)
print('keyboard.c done')
