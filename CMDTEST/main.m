//
//  main.m
//  CMDTEST
//
//  Created by Viktor Pih on 2020/11/11.
//

#import <Foundation/Foundation.h>

#import "gba.h"
#import "cpu.h"
#import "bus.h"

void print_bus(struct REBUS* bus) {
    struct Data_OP* op = &bus->op;
    printf("addr %p data %x cycles %d error %d\n", op->addr, op->data, op->cycles, op->error);
}

void regba_debug(struct REGBA* gba) {
    
    uint32_t PC = gba->cpu->prev_PC;
    char buffer[50];
    recpu_disassemble(gba->bus, PC, buffer);
    printf("[DEBUG] [%x] %s\n", PC, buffer);
    
    printf("[DEBUG] 中断信息: event %d mode %d\n", gba->event, gba->cpu->cpsr.mode);
    
    bool exit = false;
    while (!exit) {
        
        char input[50] = {0};
        
        scanf("%s", input);
        
        if (strcmp("go", input) == 0) {
            exit = true;
        } else if (strcmp("next", input) == 0) {
            regba_stop_at_next(gba);
            exit = true;
        } else if (strcmp("t0", input) == 0) {
            rebus_mem_write(gba->bus, 0x09FFFFFF-3, 0x11223344, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
            rebus_mem_read(gba->bus, 0x09FFFFFF-3, ACCESS_WIDTH_BIT_16);
            print_bus(gba->bus);
            
        } else if (strcmp("t1", input) == 0) {
            rebus_mem_write(gba->bus, 0x0203FFFF-2, 0x11223344, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
            rebus_mem_read(gba->bus, 0x0203FFFF-2, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
        } else if (strcmp("t2", input) == 0) {
            rebus_mem_write(gba->bus, 0x00003FFF-3, 0x11223344, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
            rebus_mem_read(gba->bus, 0x00003FFF-3, ACCESS_WIDTH_BIT_16);
            print_bus(gba->bus);
            
        } else if (strcmp("t3", input) == 0) {
            rebus_mem_write(gba->bus, 0x01FFFFFF-2, 0x11223344, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
            rebus_mem_read(gba->bus, 0x01FFFFFF-2, ACCESS_WIDTH_BIT_32);
            print_bus(gba->bus);
            
        } else {
            printf("%s\n", input);
        }
    }
}

void load_rom(struct REGBA* gba) {
    FILE* fp = fopen("/Users/xingbi/REGBA/gba_template/demo1/demo1.gba", "rb");
    regba_load_rom(gba, fp);
    fclose(fp);
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        
        uint32_t a = 0x11223344;
        uint16_t b  = a;
        uint8_t c = a;
        
        struct REGBA* gba = regba_create();
        regba_init(gba, &regba_debug);
        load_rom(gba);
        regba_stop_at_next(gba);
        
        regba_run(gba);
        
        regba_delete(gba);
        
        NSLog(@"Hello, World!");
    }
    return 0;
}
