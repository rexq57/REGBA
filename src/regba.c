#include "regba.h"
#include "cpu.h"
#include "bus.h"
#include "mem.h"

struct REGBA* regba_create() {
    struct REGBA* gba = (struct REGBA*)malloc(sizeof(struct REGBA));

    gba->cpu = recpu_create();
    gba->bus = rebus_create();
    gba->mem = remem_create();
    
    return gba;
}

void regba_delete(struct REGBA* gba) {
    
    recpu_delete(gba->cpu);
    rebus_delete(gba->bus);
    remem_delete(gba->mem);
    
    free(gba);
}

void regba_init(struct REGBA* gba) {
    
    remem_init(gba->mem);
    rebus_init(gba->bus, gba->mem);
    recpu_init(gba->cpu, gba->bus);
}
