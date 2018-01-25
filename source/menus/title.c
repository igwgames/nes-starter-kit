#include "tools/neslib_famitracker/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"

CODE_BANK(PRG_BANK_TITLE);

// Array of 16 colors that make up the palette for the title screen. You can generate this by using NESst, and choosing
// "put to clipboard -> c data" from the "palettes" menu.
const unsigned char titlePalette[16]={ 0x0f,0x21,0x11,0x31,0x0f,0x11,0x21,0x31,0x0f,0x05,0x25,0x35,0x0f,0x00,0x10,0x30 };

// The name of your game :)
const unsigned char gameName[] = "Quirk's Adventure";
const unsigned char gameAuthor[] = "YOU!";
const unsigned char gameAuthorContact[] = "Tweet at us! @your_twitter";
const unsigned char currentYear[] = "2018";

void draw_title_screen() {
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);
    
    put_str(NTADR_A(7, 5), gameName);
	
	put_str(NTADR_A(2, 26), gameAuthorContact);
	
	put_str(NTADR_A(2, 28), "Copyright");
	put_str(NTADR_A(12, 28), currentYear);
	put_str(NTADR_A(17, 28), gameAuthor);

	put_str(NTADR_A(10, 16), "Press Start!");
	ppu_on_all();

	gameState = GAME_STATE_TITLE_INPUT;
}

void handle_title_input() {
	if (pad_trigger(0) & PAD_START) {
		gameState = GAME_STATE_POST_TITLE;
	}
}