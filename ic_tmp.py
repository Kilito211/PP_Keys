import os
content = []
content.append('#include "interception_wrapper.h"')
content.append('#include <stdio.h>')
content.append('')
content.append('bool interception_wrapper_init(void) {')
content.append('    printf("INTCP: stub init\\n");')
content.append('    return false;')
content.append('}')
content.append('bool interception_wrapper_key_down(uint16_t vk) { (void)vk; return false; }')
content.append('bool interception_wrapper_key_up(uint16_t vk) { (void)vk; return false; }')
content.append('void interception_wrapper_deinit(void) {}')
content.append('')
with open('core/interception_wrapper.c', 'w', encoding='utf-8') as f:
    f.write('\n'.join(content))
