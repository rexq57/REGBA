#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef void (*log_callback_t)(const char*);

bool re_log(const char* format, ...);
void re_set_logcallback(log_callback_t callback);
void re_set_log(bool enabled);
