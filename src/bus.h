#pragma once

#include "type.h"

/*
 总线
 数据总线、地址总线、控制总线
 */

struct REBUS{
    
    // 数据总线
    uint32_t input, output;
    
    // 地址总线
    uint32_t addr;
    
    // 连接内存
    struct REMEM* mem;
};

struct REBUS* rebus_create(void);
void rebus_delete(struct REBUS* bus);

void rebus_init(struct REBUS* bus, struct REMEM* mem);

// I/O 接口 (控制总线)
uint32_t rebus_mem_read32bit(struct REBUS* bus, uint32_t addr);
void rebus_mem_write32bit(struct REBUS* bus, uint32_t addr, uint32_t data);
uint16_t rebus_mem_read16bit(struct REBUS* bus, uint32_t addr);
void rebus_mem_write16bit(struct REBUS* bus, uint32_t addr, uint16_t data);
