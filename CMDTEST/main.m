//
//  main.m
//  CMDTEST
//
//  Created by Viktor Pih on 2020/11/11.
//

#import <Foundation/Foundation.h>

#import "regba.h"
#import "bus.h"

void regba_debug(struct REGBA* gba) {
    
    bool exit = false;
    while (!exit) {
        
        char input[50] = {0};
        
        printf("断点信息: type %d\n", gba->interrupt);
        
        scanf("%s", input);
        
        if (strcmp("exit", input) == 0) {
            exit = true;
        } else if (strcmp("tw", input) == 0) {
            rebus_mem_write(gba->bus, 0x3fff-3, 0x11223344, ACCESS_WIDTH_BIT_32);
            printf("addr %p data %x cycles %d error %d\n", gba->bus->addr, gba->bus->data, gba->bus->cycles, gba->bus->error);
        } else if (strcmp("tr", input) == 0) {
            rebus_mem_read(gba->bus, 0x3fff-1, ACCESS_WIDTH_BIT_16);
            printf("addr %p data %x cycles %d error %d\n", gba->bus->addr, gba->bus->data, gba->bus->cycles, gba->bus->error);
        } else {
            printf("%s\n", input);
        }
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        
        uint32_t a = 0x11223344;
        uint16_t b  = a;
        uint8_t c = a;
        
        struct REGBA* gba = regba_create();
        regba_init(gba, &regba_debug);
        regba_stop_at_next(gba);
        
        regba_run(gba);
        
        regba_delete(gba);
        
        NSLog(@"Hello, World!");
    }
    return 0;
}
