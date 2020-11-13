#pragma once

#include "type.h"

/*
 总线
 数据总线、地址总线、控制总线
 */

struct REBUS{
    
    // 上一次请求的相关数据
    // 数值
    uint32_t data;
    
    // 地址
    uint32_t addr;
    
    // 周期
    int cycles;
    
    // 是否发生错误
    bool error;
    
    // 连接内存
    struct REMEM* mem;
};

struct REBUS* rebus_create(void);
void rebus_delete(struct REBUS* bus);

void rebus_init(struct REBUS* bus, struct REMEM* mem);

// I/O 接口 (控制总线)
enum ACCESS_WIDTH{
    ACCESS_WIDTH_BIT_8 = 1 << 0,
    ACCESS_WIDTH_BIT_16 = 1 << 1,
    ACCESS_WIDTH_BIT_32 = 1 << 2
};

void rebus_mem_read(struct REBUS* bus, uint32_t addr, enum ACCESS_WIDTH acc_w);
void rebus_mem_write(struct REBUS* bus, uint32_t addr, uint32_t data, enum ACCESS_WIDTH acc_w);
