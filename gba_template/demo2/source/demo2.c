
// #include <gba_console.h>
// #include <gba_video.h>
// #include <gba_interrupt.h>
// #include <gba_systemcalls.h>
// #include <gba_input.h>
// #include <stdio.h>
// #include <stdlib.h>

#include "toolbox.h"

void test() {
	*(unsigned int*)0x04000000 = 0x0403;

    ((unsigned short*)0x06000000)[120+80*240] = 0x001F;
    ((unsigned short*)0x06000000)[136+80*240] = 0x03E0;
    ((unsigned short*)0x06000000)[120+96*240] = 0x7C00;
}

void test1() {
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

    m3_plot( 120, 80, RGB15(31, 0, 0) );    // or CLR_RED
    m3_plot( 136, 80, RGB15( 0,31, 0) );    // or CLR_LIME
    m3_plot( 120, 96, RGB15( 0, 0,31) );    // or CLR_BLUE
}

// extra stuff, also in tonc_video.h
#define M3_WIDTH    SCREEN_WIDTH
// typedef for a whole mode3 line
typedef COLOR       M3LINE[M3_WIDTH];
// m3_mem is a matrix; m3_mem[y][x] is pixel (x,y)
#define m3_mem    ((M3LINE*)MEM_VRAM)

void test2() {
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2;

    m3_mem[80][120]= CLR_RED;
    m3_mem[80][136]= CLR_LIME;
    m3_mem[96][120]= CLR_BLUE;
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	test2();

    while(1);

    return 0;
}


