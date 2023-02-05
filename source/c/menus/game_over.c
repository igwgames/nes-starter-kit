#include "source/menus/game_over.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"

CODE_BANK(PRG_BANK_GAME_OVER);

void draw_game_over_screen(void) {
    ppu_off();
    clear_screen_with_border();

    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    put_str(NTADR_A(11, 13), "Game Over");



    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();

}