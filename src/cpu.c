#include "cpu.h"




struct RECPU* recpu_create(void) {
    return (struct RECPU*)malloc(sizeof(struct RECPU));
}

void recpu_delete(struct RECPU* cpu) {
    free(cpu);
}

void recpu_init(struct RECPU* cpu, struct REBUS* bus) {
    
    // 寄存器初始化
    memset(&cpu->regs, 0, sizeof(cpu->regs));
    memset(&cpu->cpsr, 0, sizeof(cpu->cpsr));
    
    // 处理器默认是用户模式
    recpu_set_mode(cpu, PROCESSOR_MODE_USER);
    
    cpu->bus = bus;
    
    cpu->current_cycle_count = 0;
};

void recpu_set_mode(struct RECPU* cpu, enum PROCESSOR_MODE mode) {
    
    // 禁止从异常模式切换到异常模式
    REGBA_ASSERT(!(cpu->cpsr.mode > PROCESSOR_MODE_SYSTEM && mode > PROCESSOR_MODE_SYSTEM));
    
    #define BANK_REGS(x, name) memcpy(&cpu->_regs_##name, &cpu->regs.r##x, sizeof(cpu->_regs_##name));
    #define RESTORE_REGS(x, name) memcpy(&cpu->regs.r##x, &cpu->_regs_##name, sizeof(cpu->_regs_##name));
    
    // 进入特殊状态时，存储寄存器
    switch (mode) {
        case PROCESSOR_MODE_USER:
        case PROCESSOR_MODE_SYSTEM:
            // 从上一个模式中恢复寄存器
            switch (cpu->cpsr.mode) {
                case PROCESSOR_MODE_IRQ:
                    RESTORE_REGS(13, IRO);
                    break;
                case PROCESSOR_MODE_FIQ:
                    RESTORE_REGS(8, FIQ);
                    break;
                case PROCESSOR_MODE_SUPERVISOR:
                    RESTORE_REGS(13, SVC);
                    break;
                case PROCESSOR_MODE_ABORT:
                    RESTORE_REGS(13, ABT);
                    break;
                case PROCESSOR_MODE_UNDEFINED:
                    RESTORE_REGS(13, UND);
                    break;
                default:
                    break;
            }
            break;
        case PROCESSOR_MODE_IRQ:
            BANK_REGS(13, IRO);
            break;
        case PROCESSOR_MODE_FIQ:
            BANK_REGS(8, FIQ);
            break;
        case PROCESSOR_MODE_SUPERVISOR:
            BANK_REGS(13, SVC);
            break;
        case PROCESSOR_MODE_ABORT:
            BANK_REGS(13, ABT);
            break;
        case PROCESSOR_MODE_UNDEFINED:
            BANK_REGS(13, UND);
            break;
        default:
            break;
    }
    
    cpu->cpsr.mode = mode;
}

int recpu_run_instruction(struct RECPU* cpu) {
    
    // 从总线上请求指令，并执行
//    rebus_mem_read32bit(gba->bus, <#uint32_t addr#>);
    
    int cycle_count = 3;
    
    cpu->current_cycle_count += cycle_count;
    
    return cycle_count;
}
