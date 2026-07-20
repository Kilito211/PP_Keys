/**
 * @file voice.c
 * @author kilito_hyx (kilito.hyx@gmail.com)
 * @brief Windows语音播报
 * @version 0.1
 * @date 2026-07-20
 *
 * @copyright Copyright (c) 2026
 *
 */

#define INITGUID
#include "voice.h"
#include <sapi.h>
#include <stdio.h>

static ISpVoice *s_voice = NULL;

/**
 * @brief 初始化语音系统
 * 
 * @return true 成功
 * @return false 失败
 */
bool voice_init(void)
{
    HRESULT hr;
    hr = CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void **)&s_voice);

    if(FAILED(hr))
    {
        printf("Voice: CocreateInstance failed,HESULT = 0x%08lX\n", hr);
        s_voice = NULL;
        return false;
    }
    printf("Voice: Init Success\n");
    return true;
}

/**
 * @brief 一部播报语音
 * 
 * @param text 要播报的文本
 * @return true 成功
 * @return false 失败
 */
bool voice_speak(const wchar_t *text)
{
    HRESULT hr;

    if(s_voice == NULL || text == NULL)
        return false;

    hr = s_voice->lpVtbl->Speak(s_voice, text, SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);

    if(FAILED(hr))
    {
        printf("Voice :Speak failed,HRESULT=0x%08lX\n", hr);
        return false;
    }

    return true;
}

/**
 * @brief 释放语音系统
 * 
 */
void voice_deinit(void)
{
    if(s_voice != NULL)
    {
        s_voice->lpVtbl->Speak(s_voice, NULL, SPF_PURGEBEFORESPEAK, NULL);
        s_voice->lpVtbl->Release(s_voice);
        s_voice = NULL;
    }
}