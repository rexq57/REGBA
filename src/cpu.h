#pragma once

/* ARM7处理器 - ARMv4架构
时钟频率16.78MHz或2 ^ 24 Hz (16777216)

 
 r0-r12：GPR（通用寄存器）
 r13：通常用作堆栈指针，如果您不使用堆栈（小型演​​示或其他内容，则此
     可以视为GPR）。
 r14：链接寄存器，可用作GPR，但其主要目的是保存分支的返回地址
     和链接指令（子例程调用）。
 r15：PC寄存器，保存当前指令的地址+8（ARM7具有3级流水线，因此如果
     阅读PC时，它的地址将是您用来阅读PC的2条指令的开头）。
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
