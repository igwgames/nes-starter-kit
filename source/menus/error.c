#include "tools/neslib_famitracker/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/menus/text_helpers.h"
#include "source/library/itoa.h"
#include "source/menus/error.h"
#include "source/graphics/palettes.h"

// Store most of our data in the given bank.
CODE_BANK(PRG_BANK_ERROR);

// Known error states for things to use.
const char* ERR_UNKNOWN_GAME_STATE = "Unknown Game State";
const char* ERR_UNKNOWN_GAME_STATE_EXPLANATION = "gameState set to a value that the main loop does not recognize.";
const char* ERR_RECURSION_DEPTH = "Bank Recursion Depth Error";
const char* ERR_RECURSION_DEPTH_EXPLANATION = "Too many requests were made to bank_call from other requests. Only up to " STR(MAX_RECURSION_DEPTH) " calls can be made.";

ZEROPAGE_ARRAY_DEF(char, buffer, 10);

// Separate function so we can put it into a separate prg bank, then call it below. See documentation with public function.
void crash_error_internal(const char *errorId, const char *errorDescription, const char* numberName, int number) {
    
    ppu_wait_frame();
    ppu_off();
    pal_bg(errorPalette);
    pal_spr(errorPalette);
    scroll(0, 0);
    oam_hide_rest(0);
    clear_screen();

    put_str(NTADR_A(1, 3), "** FATAL ERROR ENCOUNTERED **");
    put_str(NTADR_A(2, 6), errorId);
    put_str(NTADR_A(2, 10), errorDescription);

    if (numberName != NULL) {
        itoa(number, buffer);
        put_str(NTADR_A(2, 20), numberName);
        vram_put(':'-0x20);
        put_str(NTADR_A(2, 21), buffer);
    }
    put_str(NTADR_A(2, 24), "Please reset your console");
    put_str(NTADR_A(2, 25), "to continue.");
    ppu_on_all();

    // Lock all input and everything else, so we don't continue to do things after we know we've crashed.
    while (1) {}
}

CODE_BANK_POP();

// Spit out an error screen and crash the rom.
// errorId: A string describing the error. Use one of the constants in error.c/here
// errorDescription: Some text describing the error in detail. Alternatively, pass NULL.
// numberName: A description of the numeric parameter accepted after this one. Can be NULL.
// number: A number to print out that goes with the error code (address, something else...) Can be NULL.
void crash_error(const char *errorId, const char *errorDescription, const char* numberName, int number) {
    // Normally you'd want to do a bank_push/bank_pop here -- but we're about to crash the rom, so we don't care about state!
    set_prg_bank(PRG_BANK_ERROR);
    crash_error_internal(errorId, errorDescription, numberName, number);

}