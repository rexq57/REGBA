#pragma once

/* ARM7tdmi处理器 - ARMv4架构
 
时钟频率16.78MHz或2 ^ 24 Hz (16777216)
 
 r0-r12：GPR（通用寄存器）
 r13：通常用作堆栈指针，如果您不使用堆栈（小型演​​示或其他内容，则此
     可以视为GPR）。
 r14：链接寄存器，可用作GPR，但其主要目的是保存分支的返回地址
     和链接指令（子例程调用）。
 r15：PC寄存器，保存当前指令的地址+8（ARM7具有3级流水线，因此如果
     阅读PC时，它的地址将是您用来阅读PC的2条指令的开头）。
 
 设计参考：https://www.cs.rit.edu/~tjh8300/CowBite/CowBiteSpec.htm
*/

#include "type.h"

enum PROCESSOR_MODE{
    PROCESSOR_MODE_USER,
    PROCESSOR_MODE_FIQ,
    PROCESSOR_MODE_IRQ,
    PROCESSOR_MODE_SUPERVISOR,
    PROCESSOR_MODE_ABORT,
    PROCESSOR_MODE_UNDEFINED,
    PROCESSOR_MODE_SYSTEM
};

/* 当前状态寄存器
Current Program Status Register (CPSR)
  Bit   Expl.
  31    N - Sign Flag       (0=Not Signed, 1=Signed)               ;\
  30    Z - Zero Flag       (0=Not Zero, 1=Zero)                   ; Condition
  29    C - Carry Flag      (0=Borrow/No Carry, 1=Carry/No Borrow) ; Code Flags
  28    V - Overflow Flag   (0=No Overflow, 1=Overflow)            ;/
  27    Q - Sticky Overflow (1=Sticky Overflow, ARMv5TE and up only)
  26-8  Reserved            (For future use) - Do not change manually!
  7     I - IRQ disable     (0=Enable, 1=Disable)                     ;\
  6     F - FIQ disable     (0=Enable, 1=Disable)                     ; Control
  5     T - State Bit       (0=ARM, 1=THUMB) - Do not change manually!; Bits
  4-0   M4-M0 - Mode Bits   (See below)
 
 CPSR: The Current Program Status Register. This contains the status bits relevant to the CPU -

 31 30 29 28  27 26 25 24  23 22 21 20  19 18 17 16  15 14 13 12  11 10 9 8  7 6 5 4  3 2 1 0
 N  Z  C  V   R  R  R  R   R  R  R  R   R  R  R  R   R  R  R  R   R  R  R R  I F T M  M M M M
 0-4  (M) = Mode bits. These indicate the current processor mode:
            10000 - User mode
            10001 - FIQ mode
            10010 - IRQ mode
            10011 - Supervisor mode
            10111 - Abort mode
            11011 - Undefined mode
            11111 - System mode
 5    (T) = Thumb state indicator. If set, the CPU is in Thumb state.
            Otherwise it operates in normal ARM state. Software should
            never attempt to modify this bit itself.
 6    (F) = FIQ interrupt disable. Set this to disable FIQ interrupts.
 7    (I) = IRQ interrupt disable. Set this to disable IRQ interrupts. On
            the GBA this is set by default whenever IRQ mode is entered.
            Why or how this is the case, I do not know.
 8-27 (R) = Reserved
 28   (V) = Overflow condition code
 29   (C) = Carry/Borrow/Extend condition code
 30   (Z) = Zero/Equal condition code
 31   (N) = Negative/Less than condition code
 */
struct CPSR{
    enum PROCESSOR_MODE mode;
    bool T, F,
    I, // 中断禁止
    V, // 溢出
    C, // 进位
    Z, // 零
    N; // 负数
};

/* User 模式可见的寄存器
 CPU Registers
 16 registers are visible to the user at any given time, though there are 20 banked registers which get swapped in whenever the CPU changes to various priveleged modes. The registers visible in user mode are as follows:

 r0-r12: General purpose registers, for use in every day operations
 r13 (SR): Stack pointer Register. Used primarily for maintaining the address of the stack. This default value (initialized by the BIOS) differs depending on the current processor mode, as follows:

   User/System:  0x03007F00
   IRQ:          0x03007FA0
   Supervisor:   0x03007FE0
 As far as I know the other modes do not have default stack pointers.

 r14 (LR): Link Register. Used primarily to store the address following a "bl" (branch and link) instruction (as used in function calls)
 r15 (PC): The Program Counter. Because the ARM7tdmi uses a 3-stage pipeline, this register always contains an address which is 2 instructions ahead of the one currrently being executed. In 32-bit ARM state, it is 8 bytes ahead, while in 16-bit Thumb state it is 4 bytes ahead.
 */
union REGS_USER{
    uint32_t r[16]; // 提供所有寄存器的直接数组访问
    
    struct {
        
        struct {
            uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
        };
        
        // 为 r13-15 三个寄存器设置别名
        union {
            uint32_t r13;
            uint32_t SP;
        };
        
        union {
            uint32_t r14;
            uint32_t LR;
        };
        
        union {
            uint32_t r15;
            uint32_t PC;
        };
    };
};

/*
 IRQ: This mode is entered when an Interrupt Request is triggered. Any interrupt handler on the GBA will be called in IRQ mode.

 Banked registers: The ARM7tdmi has several sets of banked registers that get swapped in place of normal user mode registers when a priveleged mode is entered, to be swapped back out again once the mode is exited. In IRQ mode, r13_irq and r14_irq will be swapped in to replace r13 and r14. The current CPSR contents gets saved in the SPSR_irq register.
 */
struct REGS_IRO{
    uint32_t r13_irq, r14_irq;
    struct CPSR SPSR_irq;
};

/*
 FIQ: This mode is entered when a Fast Interrupt Request is triggered. Since all of the hardware interrupts on the GBA generate IRQs, this mode goes unused by default, though it would be possible to switch to this mode manually using the "msr" instruction.
 */
struct REGS_FIQ{
    uint32_t r8_fiq, r9_fiq, r10_fiq, r11_fiq, r12_fiq, r13_fiq, r14_fiq;
    struct CPSR SPSR_fiq;
};

/*
 SVC: Supervisor mode. Entered when a SWI (software interrupt) call is executed. The GBA enters this state when calling the BIOS via SWI instructions.
 */
struct REGS_SVC{
    uint32_t r13_svc, r14_svc;
    struct CPSR SPSR_svc;
};

/*
 ABT: Abort mode. Entered after data or instruction prefetch abort.
 */
struct REGS_ABT{
    uint32_t r13_abt, r14_abt;
    struct CPSR SPSR_abt;
};

/*
 UND: Undefined mode. Entered when an undefined instruction is executed.
 */
struct REGS_UND{
    uint32_t r13_und, r14_und;
    struct CPSR SPSR_und;
};

struct RECPU{
    
    // 用户寄存器
    union REGS_USER regs;
    
    // 各种模式下的寄存器
    struct REGS_IRO _regs_IRO;
    struct REGS_FIQ _regs_FIQ;
    struct REGS_SVC _regs_SVC;
    struct REGS_ABT _regs_ABT;
    struct REGS_UND _regs_UND;

    // 当前状态寄存器
    struct CPSR cpsr;
    
    // 总线需要暴露给CPU
    struct REBUS* bus;
    
    // 当前CPU经过的时钟周期
    unsigned int current_cycle_count;
};

struct RECPU* recpu_create(void);
void recpu_delete(struct RECPU* cpu);

void recpu_init(struct RECPU* cpu, struct REBUS* bus);

// 设置处理器状态
void recpu_set_mode(struct RECPU* cpu, enum PROCESSOR_MODE mode);

// 执行一条指令，返回所需时钟周期
int recpu_run_instruction(struct RECPU* cpu);


