#include "bus.h"
#include "mem.h"


void* memory_mapping(struct REBUS* bus, uint32_t addr, bool* writable) {
    
    void* data = 0;
    
    #define RET(a) return a;
    #define IN_RANGE(a, b) (addr >= a && addr <= b)
    
    // External Memory (Game Pak)
    if (IN_RANGE(0x0E000000, 0x0E00FFFF)) {
        // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
        RET(0);
    } else if (IN_RANGE(0x0C000000, 0x0DFFFFFF)) {
        // Game Pak ROM/FlashROM (max 32MB) - Wait State 2
        RET(0);
    } else if (IN_RANGE(0x0A000000, 0x0BFFFFFF)) {
        // Game Pak ROM/FlashROM (max 32MB) - Wait State 1
        RET(0);
    } else if (IN_RANGE(0x08000000, 0x09FFFFFF)) {
        // Game Pak ROM/FlashROM (max 32MB) - Wait State 0
        RET(0);
    }
    
    // Internal Display Memory
    if (IN_RANGE(0x07000000, 0x070003FF)) {
        // OAM - OBJ Attributes      (1 Kbyte)
        RET(bus->mem->oam);
    } else if (IN_RANGE(0x06000000, 0x06017FFF)) {
        // VRAM - Video RAM          (96 KBytes)
        RET(bus->mem->vram);
    } else if (IN_RANGE(0x05000000, 0x050003FF)) {
        // BG/OBJ Palette RAM        (1 Kbyte)
        RET(bus->mem->palette_ram);
    }
    
    return data;
}

struct REBUS* rebus_create() {
    return (struct REBUS*)malloc(sizeof(struct REBUS));
}

void rebus_delete(struct REBUS* bus) {
    free(bus);
}

void rebus_init(struct REBUS* bus, struct REMEM* mem) {
    REGBA_ASSERT(mem);
    bus->mem = mem;
}

uint32_t rebus_mem_read32bit(struct REBUS* bus, uint32_t addr) {
    REGBA_ASSERT(0);
    return 0;
}

void rebus_mem_write32bit(struct REBUS* bus, uint32_t addr, uint32_t data) {
    
    REGBA_ASSERT(0);
}

uint16_t rebus_mem_read16bit(struct REBUS* bus, uint32_t addr) {
    REGBA_ASSERT(0);
    return 0;
}

void rebus_mem_write16bit(struct REBUS* bus, uint32_t addr, uint16_t data) {
    REGBA_ASSERT(0);
}
