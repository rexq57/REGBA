#pragma once

/* ARM7处理器 - ARMv4架构
时钟频率16.78MHz或2 ^ 24 Hz (16777216)

*/

#include "type.h"

struct RECPU{
    
    union {
        uint32_t r[16]; // 提供所有寄存器的直接数组访问
        
        struct {
            
            uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
            
            // 为 r13-15 三个寄存器设置别名
            union {
                uint32_t r13;
                uint32_t sp;
            };
            
            union {
                uint32_t r14;
                uint32_t lr;
            };
            
            union {
                uint32_t r15;
                uint32_t pc;
            };
            
        };
    }regs;
    
    // 总线需要暴露给CPU
    struct REBUS* bus;
    
    // 当前CPU经过的时钟周期
    unsigned int current_cycle_count;
};

struct RECPU* recpu_create(void);
void recpu_delete(struct RECPU* cpu);

void recpu_init(struct RECPU* cpu, struct REBUS* bus);

// 执行一条指令，返回所需时钟周期
int recpu_run_instruction(struct RECPU* cpu);
