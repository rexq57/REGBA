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
        } else if (strcmp("test", input) == 0) {
            int cycles;
            bool error;
            rebus_mem_write(gba->bus, 0x0, 12345, ACCESS_WIDTH_BIT_32, &cycles, &error);
            printf("cycles %d error %d\n", cycles, error);
        } else {
            printf("%s\n", input);
        }
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        
        typedef uint8_t uint24_t[3];
        printf("uint24_t size %d\n", sizeof(uint24_t));
        
        struct REGBA* gba = regba_create();
        regba_init(gba, &regba_debug);
        regba_stop_at_next(gba);
        
        regba_run(gba);
        
        regba_delete(gba);
        
        NSLog(@"Hello, World!");
    }
    return 0;
}
