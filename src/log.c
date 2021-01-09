//
//  log.c
//  REGBA
//
//  Created by Viktor Pih on 2021/1/9.
//

#include "log.h"

char g_buffer[1024];
log_callback_t g_callback;
bool g_logEnabled = false;

static
bool _log(const char* format, ...)
{
    if (!g_logEnabled)
        return true;
    
    va_list args;
    va_start(args, format);
    vsprintf(g_buffer, format, args);
    va_end(args);
    
    if (g_callback != 0)
    {
        g_callback(g_buffer);
    }
    else
    {
        printf("%s", g_buffer);
    }
    
    return true;
}

void regba_set_logcallback(log_callback_t callback)
{
    g_callback = callback;
}


void regba_set_log(bool enabled)
{
    g_logEnabled = enabled;
}
