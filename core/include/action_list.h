#ifndef ACTION_LIST_H
#define ACTION_LIST_H

#include <stdbool.h>
#include <stdint.h>

#define ACTION_LIST_MAX_SIZE 64 //最大动作数量、V1固定数组、后续可以改为动态扩展

typedef struct macro_engine // 单项宏结构体
{
    uint16_t key;       // 虚拟键值
    uint32_t delay_ms;  // 延迟时间
} macro_action_t; 

bool action_list_init(void);

bool action_list_add(const macro_action_t *action);

uint32_t action_list_get_count(void);

const macro_action_t *action_list_get(uint32_t index);

void action_list_clear(void);

void action_list_deinit(void);

#endif // ACTION_LIST_H