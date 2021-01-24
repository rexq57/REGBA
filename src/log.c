//
//  log.c
//  REGBA
//
//  Created by Viktor Pih on 2021/1/9.
//

#include "log.h"
#include <stdlib.h>

char* g_buffer = 0;
log_callback_t g_callback;
bool g_logEnabled = true;

bool re_log(const char* format, ...)
{
    if (!g_logEnabled)
        return true;
    
    // write into g_buffer
    {
        if (!g_buffer) g_buffer = (char*)malloc(1024);
        
        va_list args;
        va_start(args, format);
        vsprintf(g_buffer, format, args);
        va_end(args);
        
        if (g_callback) {
            g_callback(g_buffer);
        } else {
            printf("%s", g_buffer);
        }
    }
    
    return true;
}

void re_set_logcallback(log_callback_t callback)
{
    g_callback = callback;
}


void re_set_log(bool enabled)
{
    g_logEnabled = enabled;
}
