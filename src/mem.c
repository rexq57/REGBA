#include "mem.h"

#include "type.h"

struct REMEM* remem_create(void) {
    struct REMEM* mem = (struct REMEM*)malloc(sizeof(struct REMEM));
    
    mem->bios_rom = 0;
    mem->work_slow_ram = 0;
    mem->work_fast_ram = 0;
    mem->io_regs = 0;
    mem->vram = 0;
    mem->oam = 0;
    mem->palette_ram = 0;
    
    return mem;
}

#define FREE(x) if (x)free(x);

void remem_delete(struct REMEM* mem) {
    
    FREE(mem->bios_rom);
    FREE(mem->work_slow_ram);
    FREE(mem->work_fast_ram);
    FREE(mem->io_regs);
    
    FREE(mem->vram);
    FREE(mem->oam);
    FREE(mem->palette_ram);
    
    free(mem);
}

void remem_init(struct REMEM* mem) {
    
    // 可以优化成一次性申请再分配
    
    mem->bios_rom = malloc(16 * 1024);
    mem->work_slow_ram = malloc(256 * 1024);
    mem->work_fast_ram = malloc(32 * 1024);
    mem->io_regs = malloc(0x3FF);
    
    mem->vram = malloc(96 * 1024);
    mem->oam = malloc(1 * 1024);
    mem->palette_ram = malloc(1 * 1024);
}
