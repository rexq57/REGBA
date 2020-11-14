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
    
    // 处理器默认是用户模式
    recpu_set_mode(cpu, PROCESSOR_MODE_USER);
    
    cpu->bus = bus;
    
    cpu->current_cycle_count = 0;
};

void recpu_set_mode(struct RECPU* cpu, enum PROCESSOR_MODE mode) {
    
    // 禁止从异常模式切换到异常模式
    REGBA_ASSERT(!(cpu->cpsr.mode > PROCESSOR_MODE_SYS && mode > PROCESSOR_MODE_SYS));
    
    // 直接拷贝数据到目标
    #define CASE_BANK_REGS(x, name) case PROCESSOR_MODE_##name: memcpy(&cpu->_regs_##name, &cpu->regs.r##x, sizeof(cpu->_regs_##name)); break;
    #define CASE_RESTORE_REGS(x, name) case PROCESSOR_MODE_##name: memcpy(&cpu->regs.r##x, &cpu->_regs_##name, sizeof(cpu->_regs_##name)); break;
    
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

int recpu_run_instruction(struct RECPU* cpu) {
    
    // 从总线上请求指令，并执行
//    rebus_mem_read32bit(gba->bus, <#uint32_t addr#>);
    
    int cycle_count = 3;
    
    cpu->current_cycle_count += cycle_count;
    
    return cycle_count;
}
