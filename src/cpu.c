#include "cpu.h"

struct RECPU* recpu_create(void) {
    return (struct RECPU*)malloc(sizeof(struct RECPU));
}

void recpu_delete(struct RECPU* cpu) {
    free(cpu);
}

void recpu_init(struct RECPU* cpu, struct REBUS* bus) {
    
    cpu->current_cycle_count = 0;
};

int recpu_run_instruction(struct RECPU* cpu) {
    
    // 从总线上请求指令，并执行
//    rebus_mem_read32bit(gba->bus, <#uint32_t addr#>);
    
    int cycle_count = 3;
    
    cpu->current_cycle_count += cycle_count;
    
    return cycle_count;
}
