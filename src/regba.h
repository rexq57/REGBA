#pragma once

#include "type.h"

// 状态
//enum EMU_STATUS{
//    EMU_STATUS_UNINITIALIZED,
//    EMU_STATUS_INITIALIZED
//};

typedef void (*REGBA_CB_DEBUG)(struct REGBA* gba);

struct REGBA{
    
    struct RECPU* cpu;
    struct REBUS* bus;
    struct REMEM* mem;
    
//    bool is_exited();
    
    // 中断
    enum InterruptType{
        InterruptTypeNone = 0,      // 无
        InterruptTypeBreakpoint,    // 断点中断，用于断点调试和单步调试
//        InterruptTypeIRQs,      // 软中断
//        InterruptTypeBreak, // 指令中断
//        InterruptTypeReset  // 重置
    };
    
    enum InterruptType interrupt;
    bool stop_at_next;
    
    REGBA_CB_DEBUG cb_debug;
    
    bool initialized; // 模拟器是否初始化的标记
};

struct REGBA* regba_create(void);
void regba_delete(struct REGBA* gba);
void regba_init(struct REGBA* gba, REGBA_CB_DEBUG cb_debug);

void regba_load_rom(struct REGBA* gba, void* rom);
void regba_stop_at_next(struct REGBA* gba);

void regba_run(struct REGBA* gba);
void regba_pause(struct REGBA* gba);
void regba_resume(struct REGBA* gba);

// 工作循环
int regba_get_error(struct REGBA* gba, char* info);
