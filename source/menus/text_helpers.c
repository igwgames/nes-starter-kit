#include "source/neslib_asm/neslib.h"
#include "source/menus/text_helpers.h"
#include "source/globals.h"

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

// Blank the screen - uses the 'space' character 
void clear_screen(void) {
	vram_adr(0x2000);
	vram_fill(' ' - 0x20, 0x0400);
}

// Clear the screen and put a nice border around it.
void clear_screen_with_border(void) {
	set_vram_update(NULL);
	vram_adr(0x2000);
	vram_fill(' ' - 0x20, 64);

	vram_put(' ' - 0x20);
	vram_put(ASCII_TILE_TL);
	vram_fill(ASCII_TILE_HORZ, 28);
	vram_put(ASCII_TILE_TR);
	vram_put(' ' - 0x20);

	for (i = 0; i != 24; ++i) {
		vram_put(' ' - 0x20);
		vram_put(ASCII_TILE_VERT);
		vram_fill(' ' - 0x20, 28);
		vram_put(ASCII_TILE_VERT);
		vram_put(' ' - 0x20);
	}
	vram_put(' ' - 0x20);
	vram_put(ASCII_TILE_BL);
	vram_fill(ASCII_TILE_HORZ, 28);
	vram_put(ASCII_TILE_BR);
	vram_put(' ' - 0x20);

	vram_fill(' ' - 0x20, 64);

	// Attribute table
	vram_fill(0, 64);

}