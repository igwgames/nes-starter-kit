#include "source/neslib_asm/neslib.h"

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

// Blank the screen - uses the 'space' character 
void clear_screen() {
	vram_adr(0x2000);
	vram_fill(' ' - 0x20, 0x0400);
}