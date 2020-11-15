#include "gba.h"
#include "cpu.h"
#include "bus.h"
#include "mem.h"

#include <unistd.h>

static inline
void process_event(struct REGBA* gba, bool interrupt_happend) {
    
    REGBA_ASSERT(gba->initialized);
    
    // 检查断点，触发事件
    if (gba->stop_at_next) {
        gba->event = SYSTEM_EVENT_BREAKPOINNT;
        gba->stop_at_next = false;
    } else {
        // 检查中断
        if (interrupt_happend) {
            gba->event = SYSTEM_EVENT_INTERRUPT;
        }
    }
    
    // 处理事件
    if (gba->event != SYSTEM_EVENT_NONE)
        gba->cb_debug(gba);
    
    gba->event = SYSTEM_EVENT_NONE;
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
    gba->event = SYSTEM_EVENT_NONE;
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
    
    // 设置PC指针起始位置
    gba->cpu->regs.PC = 0x08000000;
    
    int exit = 0;
    do {
        
        printf("[CPU] PC %p\n", gba->cpu);
        
        const enum PROCESSOR_MODE mode = gba->cpu->cpsr.mode;
        
        // 执行下一条指令
        int cycle_count = recpu_run_next_instruction(gba->cpu);
        
        // 检查模式变更
        bool interrupt_happend = false;
        const enum PROCESSOR_MODE current_mode = gba->cpu->cpsr.mode;
        if (current_mode != mode && current_mode != PROCESSOR_MODE_USER && current_mode != PROCESSOR_MODE_SYS) {
            interrupt_happend = true;
        }
        
        // 处理事件
        process_event(gba, interrupt_happend);
        
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
