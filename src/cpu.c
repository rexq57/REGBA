#include "cpu.h"
#include "bus.h"

struct RECPU* recpu_create(void) {
    return (struct RECPU*)malloc(sizeof(struct RECPU));
}

void recpu_delete(struct RECPU* cpu) {
    free(cpu);
}

void recpu_init(struct RECPU* cpu, struct REBUS* bus) {
    
    // 清空寄存器
    memset(&cpu->regs, 0, sizeof(cpu->regs));
    memset(&cpu->_regs_IRQ, 0, sizeof(cpu->_regs_IRQ));
    memset(&cpu->_regs_FIQ, 0, sizeof(cpu->_regs_FIQ));
    memset(&cpu->_regs_SVC, 0, sizeof(cpu->_regs_SVC));
    memset(&cpu->_regs_ABT, 0, sizeof(cpu->_regs_ABT));
    memset(&cpu->_regs_UND, 0, sizeof(cpu->_regs_UND));
    memset(&cpu->cpsr, 0, sizeof(cpu->cpsr));
    
    // 初始化处理器各模式下的栈针，位于工作内存中
    memset(&cpu->mode_SP, 0, sizeof(cpu->mode_SP));
    cpu->mode_SP[PROCESSOR_MODE_USER] = 0x03007F00;
    cpu->mode_SP[PROCESSOR_MODE_SYS] = 0x03007F00;
    cpu->mode_SP[PROCESSOR_MODE_IRQ] = 0x03007FA0;
    cpu->mode_SP[PROCESSOR_MODE_SVC] = 0x03007FE0;
    
    // 处理器默认是用户模式
    recpu_set_mode(cpu, PROCESSOR_MODE_USER);
    
    cpu->bus = bus;
    
    cpu->current_cycle_count = 0;
};

void recpu_set_mode(struct RECPU* cpu, enum PROCESSOR_MODE mode) {
    
    // [注意] 在某些情况下，BIOS可能允许从SWI过程内部执行中断。 如果是这样，并且如果中断处理程序进一步调用SWI，则应注意Supervisor堆栈不会溢出。
    
    // 禁止从异常模式切换到异常模式
    REGBA_ASSERT(!(cpu->cpsr.mode > PROCESSOR_MODE_SYS && mode > PROCESSOR_MODE_SYS));
    
    // 直接拷贝数据到目标
#define CASE_BANK_REGS(x, name)  \
case PROCESSOR_MODE_##name: \
memcpy(&cpu->_regs_##name, &cpu->regs.r##x, sizeof(cpu->_regs_##name)); \
break;

#define CASE_RESTORE_REGS(x, name) \
case PROCESSOR_MODE_##name: \
memcpy(&cpu->regs.r##x, &cpu->_regs_##name, sizeof(cpu->_regs_##name)); \
break;
    
    // 模式相同时，不存只取，防止初始化时，SP未定义覆盖了有效值，后面再从中取出就造成错误
    if (mode != cpu->cpsr.mode) {
        // 存储当前模式的栈针
        if (cpu->cpsr.mode == PROCESSOR_MODE_USER || cpu->cpsr.mode == PROCESSOR_MODE_SYS) {
            cpu->mode_SP[PROCESSOR_MODE_USER] = cpu->regs.SP;
            cpu->mode_SP[PROCESSOR_MODE_SYS]  = cpu->regs.SP;
        } else {
            cpu->mode_SP[cpu->cpsr.mode] = cpu->regs.SP;
        }
    }
    
    // 设置切换模式的栈针，有些模式没有栈针，就是0，读写就会报错
    cpu->regs.SP = cpu->mode_SP[mode];
    
    // 寄存器保存/恢复
    switch (mode) {
        case PROCESSOR_MODE_USER:
        case PROCESSOR_MODE_SYS:
        {
            // 从上一个模式中恢复寄存器
            switch (cpu->cpsr.mode) {
                CASE_RESTORE_REGS(13, IRQ)
                CASE_RESTORE_REGS(8, FIQ)
                CASE_RESTORE_REGS(13, SVC)
                CASE_RESTORE_REGS(13, ABT)
                CASE_RESTORE_REGS(13, UND)
                default: break;
            }
            break;
        }
        // 进入特殊模式时，存储寄存器
        CASE_BANK_REGS(13, IRQ)
        CASE_BANK_REGS(8, FIQ)
        CASE_BANK_REGS(13, SVC)
        CASE_BANK_REGS(13, ABT)
        CASE_BANK_REGS(13, UND)
        default: break;
    }
    
    // 更改当前模式
    cpu->cpsr.mode = mode;
}

int recpu_run_next_instruction(struct RECPU* cpu) {
    
    // 从内存总线上请求指令，并执行
    rebus_mem_access(cpu->bus, cpu->regs.PC, ACCESS_WIDTH_BIT_32);
    
    // 理论上指令读取不会出错
    REGBA_ASSERT(cpu->bus->error == MEM_ERROR_NONE);
    
    // uint32_t instruction
    
    int cycle_count = 3;
    
    cpu->current_cycle_count += cycle_count;
    
    return cycle_count;
}
