#include "source/c/menus/credits.h"
#include "graphics/palettes/palettes.config.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"
#include "source/c/configuration/game_states.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/configuration/game_info.h"

CODE_BANK(PRG_BANK_CREDITS_MENU);

void draw_win_screen(void) {
    set_vram_update(NULL);
    ppu_off();
    clear_screen();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
    pal_spr(titlePalette);
    scroll(0, 0);

    set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(7, 2), "- Congratulations -");

    put_str(NTADR_A(4, 8), "You did the thing, and");
    put_str(NTADR_A(4, 9), "thus won the game!");

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

}

void draw_credits_screen(void) {
    set_vram_update(NULL);
    ppu_off();
    clear_screen();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
    pal_spr(titlePalette);
    scroll(0, 0);

    set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(11, 2), "- Credits -");

    put_str(NTADR_A(2, 6), "Game Design and Logic");
    put_str(NTADR_A(4, 8), gameAuthor);

    put_str(NTADR_A(2, 11), "Music");
    put_str(NTADR_A(4, 13), "Wolfgang (OpenGameArt)");

    put_str(NTADR_A(2, 16), "Artwork");
    put_str(NTADR_A(4, 18), "Refresh Games (OpenGameArt)");

    put_str(NTADR_A(6, 24), "Created in");
    put_str(NTADR_A(17, 24), currentYear);
    put_str(NTADR_A(22, 24), "by");

    put_str(NTADR_A(8, 26), gameAuthor);


    // Hide all existing sprites
    oam_clear();
    ppu_on_all();
}