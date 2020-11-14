#include "gba.h"
#include "cpu.h"
#include "bus.h"
#include "mem.h"

#include <unistd.h>

void process_interrupt(struct REGBA* gba) {
    
    REGBA_ASSERT(gba->initialized);
    
    // 检查断点，触发
    if (gba->stop_at_next) {
        // 触发CPU中断
        gba->interrupt = InterruptTypeBreakpoint;
        gba->stop_at_next = false;
    }
    
    switch (gba->interrupt) {
        case InterruptTypeBreakpoint:
            gba->cb_debug(gba);
            break;
        default:
            break;
    }
}


struct REGBA* regba_create() {
    struct REGBA* gba = (struct REGBA*)malloc(sizeof(struct REGBA));

    gba->cpu = recpu_create();
    gba->bus = rebus_create();
    gba->mem = remem_create();
    
    gba->initialized = false;
    
    return gba;
}

void regba_delete(struct REGBA* gba) {
    
    recpu_delete(gba->cpu);
    rebus_delete(gba->bus);
    remem_delete(gba->mem);
    
    free(gba);
}

void regba_init(struct REGBA* gba, REGBA_CB_DEBUG cb_debug) {
    
    // 初始化硬件
    remem_init(gba->mem);
    rebus_init(gba->bus, gba->mem);
    recpu_init(gba->cpu, gba->bus);
    
    // 一些状态初始化
    gba->interrupt = InterruptTypeNone;
    gba->stop_at_next = false;
    
    gba->cb_debug = cb_debug;
    
    gba->initialized = true;
}

void regba_load_rom(struct REGBA* gba, FILE* fp) {
    
    REGBA_ASSERT(gba->initialized);
    
    // 载入(虚拟)内存中
    uint8_t* buf = gba->mem->gamepak_rom[0];
    while(!feof(fp)) {
        buf += fread(buf, 1024*1024, 1, fp);
    }
}

void regba_stop_at_next(struct REGBA* gba) {
    
    REGBA_ASSERT(gba->initialized);
    
    gba->stop_at_next = true;
}

void regba_run(struct REGBA* gba) {
    
    REGBA_ASSERT(gba->initialized);
    
    int exit = 0;
    do {
        
        // 循环
        int cycle_count = recpu_run_instruction(gba->cpu);
        
        // 处理中断
        process_interrupt(gba);
        
        usleep(1000 * 1000);
        
    } while (!exit);
}

void regba_pause(struct REGBA* gba) {
    
    REGBA_ASSERT(0);
}

void regba_resume(struct REGBA* gba) {
    
    REGBA_ASSERT(0);
}

int regba_get_error(struct REGBA* gba, char* info) {
    
    return 0;
}
