#include "bus.h"
#include "mem.h"
#include <string.h>

// 总线带宽和访问周期设置: http://problemkaputt.de/gbatek.htm#gbamemorymap
//    Address Bus Width and CPU Read/Write Access Widths
//    Shows the Bus-Width, supported read and write widths, and the clock cycles for 8/16/32bit accesses.
//      Region        Bus   Read      Write     Cycles
//      BIOS ROM      32    8/16/32   -         1/1/1
//      Work RAM 32K  32    8/16/32   8/16/32   1/1/1
//      I/O           32    8/16/32   8/16/32   1/1/1
//      OAM           32    8/16/32   16/32     1/1/1 *
//      Work RAM 256K 16    8/16/32   8/16/32   3/3/6 **
//      Palette RAM   16    8/16/32   16/32     1/1/2 *
//      VRAM          16    8/16/32   16/32     1/1/2 *
//      GamePak ROM   16    8/16/32   -         5/5/8 **/***
//      GamePak Flash 16    8/16/32   16/32     5/5/8 **/***
//      GamePak SRAM  8     8         8         5     **
//    Timing Notes:
//      *   Plus 1 cycle if GBA accesses video memory at the same time.
//      **  Default waitstate settings, see System Control chapter.
//      *** Separate timings for sequential, and non-sequential accesses.
//      One cycle equals approx. 59.59ns (ie. 16.78MHz clock).
//    All memory (except GamePak SRAM) can be accessed by 16bit and 32bit DMA.

/*
 内存映射
 General Internal Memory
   00000000-00003FFF   BIOS - System ROM         (16 KBytes)
   00004000-01FFFFFF   Not used
   02000000-0203FFFF   WRAM - On-board Work RAM  (256 KBytes) 2 Wait
   02040000-02FFFFFF   Not used
   03000000-03007FFF   WRAM - On-chip Work RAM   (32 KBytes)
   03008000-03FFFFFF   Not used
   04000000-040003FE   I/O Registers
   04000400-04FFFFFF   Not used
 Internal Display Memory
   05000000-050003FF   BG/OBJ Palette RAM        (1 Kbyte)
   05000400-05FFFFFF   Not used
   06000000-06017FFF   VRAM - Video RAM          (96 KBytes)
   06018000-06FFFFFF   Not used
   07000000-070003FF   OAM - OBJ Attributes      (1 Kbyte)
   07000400-07FFFFFF   Not used
 External Memory (Game Pak)
   08000000-09FFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 0
   0A000000-0BFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 1
   0C000000-0DFFFFFF   Game Pak ROM/FlashROM (max 32MB) - Wait State 2
   0E000000-0E00FFFF   Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
   0E010000-0FFFFFFF   Not used
 Unused Memory Area
   10000000-FFFFFFFF   Not used (upper 4bits of address bus unused)
 */

struct BusAccess {
    int bus_width;
    int read_width;
    int write_width;
    char cycles[3];
};

#define ACCESS_WIDTH_BIT_FULL ACCESS_WIDTH_BIT_8|ACCESS_WIDTH_BIT_16|ACCESS_WIDTH_BIT_32
#define ACCESS_WIDTH_BIT_16_32 ACCESS_WIDTH_BIT_16|ACCESS_WIDTH_BIT_32

static struct BusAccess bus_access[] = {
    /* BIOS ROM */      {32, ACCESS_WIDTH_BIT_FULL, 0, "111"},
    /* Work RAM 32K */  {32, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_FULL, "111"},
    /* I/O */           {32, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_FULL, "111"},
    /* OAM */           {32, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_16_32, "111"},
    /* Work RAM 256K */ {16, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_FULL, "336"},
    /* Palette RAM */   {16, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_16_32, "112"},
    /* VRAM */          {16, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_16_32, "112"},
    /* GamePak ROM */   {16, ACCESS_WIDTH_BIT_FULL, 0, "558"},
    /* GamePak Flash */ {16, ACCESS_WIDTH_BIT_FULL, ACCESS_WIDTH_BIT_16_32, "558"},
    /* GamePak SRAM */  {8, ACCESS_WIDTH_BIT_8, ACCESS_WIDTH_BIT_8, "500"},
};

enum {
    BIOS_ROM,
    Work_RAM_32K,
    I_O,
    OAM,
    Work_RAM_256K,
    Palette_RAM,
    VRAM,
    GamePak_ROM,
    GamePak_Flash, // 不使用，默认为是ROM，全不可写
    GamePak_SRAM
};

const struct BusAccess* memory_mapping(struct REBUS* bus, uint32_t addr, void** data, int* size) {
    
    #define RET(a) return &bus_access[a];
    #define IN_RANGE(a, b, c, d) if (addr >= a && addr <= b) {/*printf("base=%p offset=%d %p %p %d\n", d, addr-a, b, addr, b-addr);*/*data=d+(addr-a);*size=b-addr+1;RET(c);}
    
    // External Memory (Game Pak)
    IN_RANGE(0x0E000000, 0x0E00FFFF, GamePak_SRAM, bus->mem->gamepak_sram)  // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
    IN_RANGE(0x0C000000, 0x0DFFFFFF, GamePak_ROM, bus->mem->gamepak_rom[0]) // Game Pak ROM/FlashROM (max 32MB) - Wait State 2
    IN_RANGE(0x0A000000, 0x0BFFFFFF, GamePak_ROM, bus->mem->gamepak_rom[1]) // Game Pak ROM/FlashROM (max 32MB) - Wait State 1
    IN_RANGE(0x08000000, 0x09FFFFFF, GamePak_ROM, bus->mem->gamepak_rom[2]) // Game Pak ROM/FlashROM (max 32MB) - Wait State 0
    
    // Internal Display Memory
    IN_RANGE(0x07000000, 0x070003FF, OAM, bus->mem->oam)                    // OAM - OBJ Attributes      (1 Kbyte)
    IN_RANGE(0x06000000, 0x06017FFF, VRAM, bus->mem->vram)                  // VRAM - Video RAM          (96 KBytes)
    IN_RANGE(0x05000000, 0x050003FF, Palette_RAM, bus->mem->palette_ram)    // BG/OBJ Palette RAM        (1 Kbyte)
    
    // General Internal Memory
    IN_RANGE(0x04000000, 0x040003FE, I_O, bus->mem->io_regs)                    // I/O Registers
    IN_RANGE(0x03000000, 0x03007FFF, Work_RAM_32K, bus->mem->work_32k_ram)      // WRAM - On-chip Work RAM   (32 KBytes)
    IN_RANGE(0x02000000, 0x0203FFFF, Work_RAM_256K, bus->mem->work_256k_ram)    // WRAM - On-board Work RAM  (256 KBytes) 2 Wait
    IN_RANGE(0x00000000, 0x00003FFF, BIOS_ROM, bus->mem->bios_rom)              // BIOS - System ROM         (16 KBytes)
    
    return 0;
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

void rebus_mem_access(struct REBUS* bus, uint32_t addr, enum ACCESS_WIDTH acc_w) {
    
    // 读取数据可采用遮罩方式来屏蔽无效的位
    void* data;
    int size;
    int cycles = 0;
    const struct BusAccess* acc = memory_mapping(bus, addr, &data, &size);
    uint32_t output = 0;
    
    if (acc) {
        
        cycles = acc->cycles[(int)acc_w>>1] - '0';
        
        if (acc->read_width & acc_w) {
            
            const int acc_w_bytes = (int)acc_w;
            const int read_size = acc_w_bytes < size ? acc_w_bytes : size;
            
            switch (read_size) {
                case 1:
                    output = *(uint8_t*)data;
                    break;
                case 2:
                    output = *(uint16_t*)data;
                    break;
                case 3:
                    // output = (*(uint32_t*)data) & 0xffffff; // 可能会触发真正的内存访问错误
                    output = *(uint16_t*)data + (((uint8_t*)data)[2] << 16);
                    break;
                case 4:
                    output = *(uint32_t*)data;
                    break;
                default:
                    assert(!"error");
                    break;
            }
            
            if (read_size == acc_w) {
                bus->error = MEM_ERROR_NONE;
            } else {
                bus->error = MEM_ERROR_INCOMPLETE;
            }
            
        } else {
            bus->error = MEM_ERROR_WIDTH_LESS;
        }
    } else {
        bus->error = MEM_ERROR_INVALID_ADDR;
    }
    
    bus->addr   = addr;
    bus->data   = output;
    bus->cycles = cycles;
}

void rebus_mem_read(struct REBUS* bus, uint32_t addr, enum ACCESS_WIDTH acc_w) {
    
    // BIOS内存不可读
    if (addr >= 0 && addr <= 0x01FFFFFF) {
        bus->error = MEM_ERROR_INVALID_ADDR;
        bus->addr   = addr;
        bus->data   = 0;
        bus->cycles = 0;
    } else {
        rebus_mem_access(bus, addr, acc_w);
    }
}

void rebus_mem_write(struct REBUS* bus, uint32_t addr, uint32_t value, enum ACCESS_WIDTH acc_w) {
    
    void* data;
    int size;
    int cycles = 0;
    const struct BusAccess* acc = memory_mapping(bus, addr, &data, &size);
    if (acc) {
        cycles = acc->cycles[(int)acc_w>>1] - '0';
        
        if (acc->write_width & acc_w) {

            const int acc_w_bytes = (int)acc_w;
            const int write_size = acc_w_bytes < size ? acc_w_bytes : size;
            
            switch (write_size) {
                case 1:
                    (*(uint8_t*)data) = value;// & 0xff;
                    break;
                case 2:
                    (*(uint16_t*)data) = value;// & 0xffff;
                    break;
                case 3:
                    // 通常此操作是由于32bit写入在有效内存末端被截断所致
                    ((uint16_t*)data)[0] = ((uint16_t*)&value)[0];
                    ((uint8_t*)data)[2] = ((uint8_t*)&value)[2];
                    // memcpy(data, &value, 3);
                    break;
                case 4:
                    (*(uint32_t*)data) = value;
                    break;
                default:
                    assert(!"error");
                    break;
            }
            
            if (write_size == acc_w) {
                bus->error = MEM_ERROR_NONE;
            } else {
                bus->error = MEM_ERROR_INCOMPLETE;
            }
            
        } else {
            bus->error = MEM_ERROR_WIDTH_LESS;
        }
    } else {
        bus->error = MEM_ERROR_INVALID_ADDR;
    }
    
    bus->addr = addr;
    bus->data = value;
    bus->cycles = cycles;
}
