#pragma once

#include "type.h"

struct REBUS{
    
    // 总线需要存储读写地址、输入/输出的数据
    uint32_t input, output;
    uint32_t addr;
    
    struct REMEM* mem;
};

struct REBUS* rebus_create(void);
void rebus_delete(struct REBUS* bus);

void rebus_init(struct REBUS* bus, struct REMEM* mem);

// I/O 接口
uint32_t rebus_mem_read32bit(struct REBUS* bus, uint32_t addr);
void rebus_mem_write32bit(struct REBUS* bus, uint32_t addr, uint32_t data);
