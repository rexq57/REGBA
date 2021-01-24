#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "log.h"

#ifdef DEBUG
#define REGBA_ASSERT(...) assert(__VA_ARGS__)
#define REGBA_DEBUG
#else
#define REGBA_ASSERT(...)
#endif

#ifndef REGBA_DEBUG
#define re_log(...)
#endif

#define MEM_GAMEPAK_ROM 0x08000000
