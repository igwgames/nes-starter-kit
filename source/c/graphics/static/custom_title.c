#include "source/c/graphics/static/custom_title.h"
#include "graphics/static/custom_title_assets.h"
#include "source/c/configuration/game_states.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"

void draw_custom_title(void) {
    ppu_off();
    
    // Select our set of chr tiles
    set_chr_bank_0(CHR_BANK_CUSTOM_TITLE);
    set_chr_bank_1(CHR_BANK_CUSTOM_TITLE);

    // Remove any stray sprites
    oam_clear();

    // Set the palettes
    pal_bg(custom_title_palette);
    pal_spr(custom_title_palette);

    // Load up the nametable, and uncompress it onto the screen.
    vram_unrle(custom_title_nametable_rle);

    // We're done; show it to the user and move on.
    ppu_on_all();
    gameState = GAME_STATE_TITLE_INPUT;
}

void handle_custom_title_input(void) {
    if (pad_trigger(0) & PAD_START) {
		gameState = GAME_STATE_POST_TITLE;
	}
}