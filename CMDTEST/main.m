//
//  main.m
//  CMDTEST
//
//  Created by Viktor Pih on 2020/11/11.
//

#import <Foundation/Foundation.h>

#import "regba.h"

void regba_debug(struct REGBA* gba) {
    
    bool exit = false;
    while (!exit) {
        
        char input[50];
        
        printf("断点信息: type %d\n", gba->interrupt);
        
        
        scanf("%s", input);
        
        if (strcmp("exit", input) == 0) {
            exit = true;
        } else {
            printf("%s\n", input);
        }
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        
        struct REGBA* gba = regba_create();
        regba_init(gba, &regba_debug);
        regba_stop_at_next(gba);
        
        regba_run(gba);
        
        regba_delete(gba);
        
        NSLog(@"Hello, World!");
    }
    return 0;
}
