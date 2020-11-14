#include "mem.h"

#include "type.h"

#include <mach/mach.h>

#define check assert

void* AllocateVirtualMemory(size_t size)
{
    char*          data;
    kern_return_t   err;
    
    // In debug builds, check that we have
    // correct VM page alignment
    // check(size != 0);
    check((size % 4096) == 0); // 似乎非4096也一样可用
    
    // Allocate directly from VM
    err = vm_allocate(  (vm_map_t) mach_task_self(),
                      (vm_address_t*) &data,
                      size,
                      VM_FLAGS_ANYWHERE);
    
    // Check errors
    check(err == KERN_SUCCESS);
    if(err != KERN_SUCCESS)
    {
        data = NULL;
    }
    
    return data;
}

struct ADDR_SIZE{
    void** addr;
    size_t size;
};

void re_malloc(struct ADDR_SIZE* addr_list, int count, bool vm) {
    
    // 统计需要申请的内存总数
    size_t total = 0;
    for (int i=0; i<count; i++) {
        total += addr_list[i].size;
    }
    
    void* base;
    if (vm) {
        base = AllocateVirtualMemory(total);
    } else {
        base = malloc(total);
    }
    
    // 分配地址
    *addr_list[0].addr = base;
    for (int i=1; i<count; i++) {
        *addr_list[i].addr = (void*)((uint8_t*)*addr_list[i-1].addr + addr_list[i-1].size);
    }
    
    // check
#ifdef DEBUG
    printf("[re_malloc] 申请内存 %zu 虚拟内存 %d\n", total, vm);
    for (int i=0; i<count; i++) {
        printf("[%d]addr %p\n", i, *addr_list[i].addr);
    }
#endif
}

struct REMEM* remem_create(void) {
    struct REMEM* mem = (struct REMEM*)malloc(sizeof(struct REMEM));
    
    mem->bios_rom = 0;
    mem->work_256k_ram = 0;
    mem->work_32k_ram = 0;
    mem->io_regs = 0;
    mem->vram = 0;
    mem->oam = 0;
    mem->palette_ram = 0;
    
    return mem;
}

#define FREE(x) if (x)free(x);

void remem_delete(struct REMEM* mem) {
    
    FREE(mem->bios_rom);
    FREE(mem->work_256k_ram);
    FREE(mem->work_32k_ram);
    FREE(mem->io_regs);
    
    FREE(mem->vram);
    FREE(mem->oam);
    FREE(mem->palette_ram);
    
    free(mem);
}

void remem_init(struct REMEM* mem) {
    
    /*
    mem->bios_rom = malloc(16 * 1024);
    mem->work_256k_ram = malloc(256 * 1024);
    mem->work_32k_ram = malloc(32 * 1024);
    mem->io_regs = malloc(0x3FF);
    
    mem->vram = malloc(96 * 1024);
    mem->oam = malloc(1 * 1024);
    mem->palette_ram = malloc(1 * 1024);
    
    mem->gamepak_rom[0] = malloc(32 * 1024 * 1024);
    mem->gamepak_rom[1] = malloc(32 * 1024 * 1024);
    mem->gamepak_rom[2] = malloc(32 * 1024 * 1024);
    
    mem->gamepak_sram = malloc(64 * 1024);
     */
    
    // 优化成连续内存
    
    struct ADDR_SIZE mem_list[] = {
        {&mem->bios_rom, 16 * 1024},
        {&mem->work_256k_ram, 256 * 1024},
        {&mem->work_32k_ram, 32 * 1024},
        {&mem->io_regs, 0x3FF},
        {&mem->palette_ram, 1 * 1024},
        {&mem->vram, 96 * 1024},
        {&mem->oam, 1 * 1024},
    };
    
    re_malloc(mem_list, sizeof(mem_list)/sizeof(*mem_list), false);
    
    struct ADDR_SIZE vm_list[] = {
        {&mem->gamepak_rom[0], 32 * 1024 * 1024},
        //{&mem->gamepak_rom[1], 32 * 1024 * 1024}, 这两个是镜像，所以不用申请内存，在下面直接使用镜像地址
        //{&mem->gamepak_rom[2], 32 * 1024 * 1024},
        {&mem->gamepak_sram, 64 * 1024},
    };
    
    re_malloc(vm_list, sizeof(vm_list)/sizeof(*vm_list), true);
    
    // 镜像内存
    mem->gamepak_rom[1] = mem->gamepak_rom[0];
    mem->gamepak_rom[2] = mem->gamepak_rom[0];
    
    // RAM清零，只处理IO，PAL，VRAM和OAM
    for (int i=3; i<sizeof(mem_list)/sizeof(*mem_list); i++) {
        memset(*mem_list[i].addr, 0, mem_list[i].size);
    }
}
