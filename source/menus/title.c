#include "tools/neslib_famitracker/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"

CODE_BANK(PRG_BANK_TITLE)

// TODO: Make this easy to replace
const unsigned char titlePalette[16]={ 0x0f,0x21,0x11,0x31,0x0f,0x11,0x21,0x31,0x0f,0x05,0x25,0x35,0x0f,0x00,0x10,0x30 };

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void draw_title_screen() {
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);
    
    put_str(NTADR_A(5, 5), "HELLO WORLD?");
	ppu_on_all();

	gameState = GAME_STATE_TITLE_INPUT;
}

void handle_title_input() {

}

