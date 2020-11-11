#pragma once

/*
 Internal Memory
   BIOS ROM     16 KBytes
   Work RAM     288 KBytes (Fast 32K on-chip, plus Slow 256K on-board)
   VRAM         96 KBytes
   OAM          1 KByte (128 OBJs 3x16bit, 32 OBJ-Rotation/Scalings 4x16bit)
   Palette RAM  1 KByte (256 BG colors, 256 OBJ colors)
 */

struct REMEM{
    
    void* bios_rom;
    void* work_slow_ram;
    void* work_fast_ram;
    void* io_regs;
    
    void* vram;
    void* oam;
    void* palette_ram;
};

struct REMEM* remem_create(void);
void remem_delete(struct REMEM* mem);

void remem_init(struct REMEM* mem);
