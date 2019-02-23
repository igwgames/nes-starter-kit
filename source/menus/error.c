#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/menus/text_helpers.h"
#include "source/library/itoa.h"
#include "source/menus/error.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"

// Store most of our data in the given bank.
CODE_BANK(PRG_BANK_ERROR);

// Known error states for things to use.
const char* ERR_UNKNOWN_GAME_STATE = "Unknown Game State";
const char* ERR_UNKNOWN_GAME_STATE_EXPLANATION = "gameState set to a value that the main loop does not recognize.";
const char* ERR_RECURSION_DEPTH = "Bank Recursion Depth Error";
const char* ERR_RECURSION_DEPTH_EXPLANATION = "Too many requests were made to bank_call from other requests. Only up to " STR(MAX_RECURSION_DEPTH) " calls can be made.";
const char* ERR_RECURSION_DEPTH_VAR = "MAX_BANK_DEPTH";
const char* ERR_UNKNOWN_SPRITE_SIZE = "Unknown Sprite Size";
const char* ERR_UNKNOWN_SPRITE_SIZE_EXPLANATION = "A sprite definition has a size that the engine does not recognize.";
const char* ERR_GAME_TEXT_MISSING = "Game Text Missing";
const char* ERR_GAME_TEXT_MISSING_EXPLANATION = "No game text was specified before calling draw_game_text. This is done  the trigger_game_text method.";

char buffer[10];

// Separate function so we can put it into a separate prg bank, then call it below. See documentation with public function.
void _print_static_screen(void) {
    set_vram_update(NULL);
    pal_bright(4);

    ppu_wait_frame();
    ppu_off();
    set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
    pal_bg(errorPalette);
    pal_spr(errorPalette);
    scroll(0, 0);
    oam_hide_rest(0);
    clear_screen();

    put_str(NTADR_A(1, 3), "** FATAL ERROR ENCOUNTERED **");
    put_str(NTADR_A(2, 24), "Please reset your console");
    put_str(NTADR_A(2, 25), "to continue.");

}

CODE_BANK_POP();

// Private function in primary bank to print the variable data, which could live in another bank
void _print_variable_data_and_exit(const char *errorId, const char *errorDescription, const char* numberName, int number) {
    put_str(NTADR_A(2, 6), errorId);
    put_str(NTADR_A(2, 10), errorDescription);

    if (numberName != NULL) {
        itoa(number, buffer);
        put_str(NTADR_A(2, 20), numberName);
        vram_put(':'-0x20);
        put_str(NTADR_A(2, 21), buffer);
    }
    
    // Turn the screen back on
    ppu_on_all();

    // Lock all input and everything else, so we don't continue to do things after we know we've crashed.
    while (1) {}
}

// Public/useful methods below:

// Spit out an error screen and crash the rom. Can be passed arbitrary strings. (Note: Does NOT have access to ERR_* constants in this file. Use the method below instead.)
// errorId: A string describing the error. Use one of the constants in error.c/here
// errorDescription: Some text describing the error in detail. Alternatively, pass NULL.
// numberName: A description of the numeric parameter accepted after this one. Can be NULL.
// number: A number to print out that goes with the error code (address, something else...) Can be NULL.
void crash_error(const char *errorId, const char *errorDescription, const char* numberName, int number) {
    bank_push(PRG_BANK_ERROR);
    _print_static_screen();
    bank_pop();
    _print_variable_data_and_exit(errorId, errorDescription, numberName, number);
}

// Same method above with the same signature, BUT, this one switches the PRG bank first, allowing you to use the
// ERR_* variables built into this file.
void crash_error_use_banked_details(const char *errorId, const char *errorDescription, const char* numberName, int number) {
    set_prg_bank(PRG_BANK_ERROR);
    _print_static_screen();
    _print_variable_data_and_exit(errorId, errorDescription, numberName, number);
}