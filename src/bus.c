#include "bus.h"

struct REBUS* rebus_create() {
    return (struct REBUS*)malloc(sizeof(struct REBUS));
}

void rebus_delete(struct REBUS* bus) {
    free(bus);
}

void rebus_init(struct REBUS* bus, struct REMEM* mem) {
    bus->mem = mem;
}

uint32_t rebus_mem_read32bit(struct REBUS* bus, uint32_t addr) {
    REGBA_ASSERT(0);
    return 0;
}

void rebus_mem_write32bit(struct REBUS* bus, uint32_t addr, uint32_t data) {
    
    REGBA_ASSERT(0);
}
