#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <assert.h>

#ifdef DEBUG
#define REGBA_ASSERT(...) assert(__VA_ARGS__)
#define RENES_DEBUG
#else
#define REGBA_ASSERT(...)
#endif

#define MEM_GAMEPAK_ROM 0x08000000
