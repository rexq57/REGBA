#pragma once

#include <stdio.h>
#include <stdarg.h>

#ifdef REGBA_DEBUG
#define log(...) _log(__VA_ARGS__)
#else
#define log(...)
#endif

typedef void (*log_callback_t)(const char*);

void regba_set_logcallback(log_callback_t callback);
void regba_set_log(bool enabled);
