#pragma once

struct REGBA{
    
    struct RECPU* cpu;
    struct REBUS* bus;
    struct REMEM* mem;
    
};

struct REGBA* regba_create(void);
void regba_delete(struct REGBA* gba);
void regba_init(struct REGBA* gba);
