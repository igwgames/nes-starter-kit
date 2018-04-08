#include "source/menus/credits.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"

CODE_BANK(PRG_BANK_CREDITS_MENU);

void draw_win_screen() {
    ppu_off();
    clear_screen();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU+1);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(7, 2), "- Congratulations -");

    put_str(NTADR_A(4, 8), "You did the thing, and");
    put_str(NTADR_A(4, 9), "thus won the game!");

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

}

void draw_credits_screen() {
    ppu_off();
    clear_screen();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

    // TODO: Do we wanna switch around mirroring, and do a nice slow scroll to the bottom?
    // Bonus points: write as we go, start with a black starwars-esque screen

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU+1);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(11, 2), "- Credits -");

    put_str(NTADR_A(2, 6), "Game Design and Logic");
    // FIXME: Constants. Update guide to match for 1st change.
    put_str(NTADR_A(4, 8), "Your name here");

    put_str(NTADR_A(2, 11), "Music");
    put_str(NTADR_A(4, 13), "Wolfgang (OpenGameArt)");

    put_str(NTADR_A(2, 16), "Artwork");
    put_str(NTADR_A(4, 18), "Refresh Games (OpenGameArt)");

    // FIXME: Where's that constant?
    // FIXME: Need a way to bring this about
    // TODO: How can we make this easy to expand on?
    put_str(NTADR_A(6, 24), "Created in 2018 by");
    put_str(NTADR_A(8, 26), "Your name here");


    // Hide all existing sprites
    oam_clear();
    ppu_on_all();
}

void handle_credits_input() {
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            gameState = GAME_STATE_SYSTEM_INIT;
            break;
        }

    }
}