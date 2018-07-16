#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/configuration/game_states.h"
#include "source/library/bank_helpers.h"
#include "source/menus/error.h"
#include "source/graphics/game_text.h"
#include "source/graphics/hud.h"

unsigned char* currentText;
unsigned char currentBank;

#define haveHitNull tempChar1
#define bufferIndex tempChar2
#define hasInput tempChar3
#define currentChar tempChar4
#define stringIndex tempInt1

void trigger_game_text(const unsigned char* string) {
    gameState = GAME_STATE_SHOWING_TEXT;
    currentText = (unsigned char*)string;
    currentBank = get_prg_bank();
}

// private method in the primary bank to load characters from the bank the text resides in. We have to keep
// this separate so we can limit how much code we put into the primary bank.
void set_char_at_buffer_index() {
    bank_push(currentBank);
    currentChar = currentText[stringIndex];
    bank_pop();
}

CODE_BANK(PRG_BANK_GAME_TEXT);
void draw_game_text() {
    // Creating a local variable on the stack for the HUD buffer here. It won't be fast, but we don't really need it to be.
    unsigned char buffer[132];

    // Make sure we don't update the HUD at all while doing this.
    set_vram_update(NULL);

    if (currentText == NULL) {
        crash_error_use_banked_details(ERR_GAME_TEXT_MISSING, ERR_GAME_TEXT_MISSING_EXPLANATION, NULL, NULL);
    }
    // Draw sprite0 onto the screen so we can test it.
    oam_spr(249, HUD_PIXEL_HEIGHT-NES_SPRITE_HEIGHT-0, HUD_SPRITE_ZERO_TILE_ID, 0x00, 0);
    // And set the chr bank to use prior.

    // First, we clear the HUD using our current HUD tiles. Note that this tile must be blank in both chr banks.
    // Any shared HUD elements (borders, etc) must also be present in both layouts.
    buffer[0] = MSB(NAMETABLE_A + HUD_POSITION_START + SCREEN_WIDTH_TILES*2) | NT_UPD_HORZ;
    buffer[1] = LSB(NAMETABLE_A + HUD_POSITION_START + SCREEN_WIDTH_TILES*2);
    buffer[2] = 96;
    // We reproduce this loop a few times because we want to stick with the local stack-based variable.
    for (i = 3; i != 99; ++i) {
        buffer[i] = HUD_TILE_BLANK;
    }
    buffer[i] = NT_UPD_EOF;
    
    // This triggers drawing all the blanks we queued up.
    set_vram_update(buffer);

    // This sets a flag to use CHR_BANK_MENU at the start of every screen. After turning this on we must change the bank
    // every frame after we hit sprite0, or we risk glitches.
    set_nmi_chr_tile_bank(CHR_BANK_MENU);
    ppu_wait_nmi();
    wait_for_sprite0_hit();
    set_chr_bank_0(CHR_BANK_TILES);
    // Drawing done; make sure we don't try to keep drawing it to save some time.
    set_vram_update(NULL);

    // Ok, now we build up the text of the actual thing.
    haveHitNull = FALSE;
    stringIndex = 0;

    // The user hasn't actually done any input this time, but this forces a draw of the text, which is what we want. 
    hasInput = TRUE; 

    // Next, keep drawing text until we hit a null character.
    while (1) {

        // If the player pressed A, draw the next line and/or exit.
        if (hasInput) {
            hasInput = FALSE;
            // If we've gotten to the end of the text, stop. 
            if (haveHitNull) {
                break;
            }

            // Else, draw the next 3 lines.
            // First, clear the buffer out so any blank characters stay blank.
            bufferIndex = 4;
            for (i = 3; i != 99; ++i) {
                buffer[i] = HUD_TILE_BLANK;
            }

            // Quick break for nmi to make sure we don't have glitches
            ppu_wait_nmi();
            wait_for_sprite0_hit();
            set_chr_bank_0(CHR_BANK_TILES);

            // Draw 3 lines of text.
            for (; bufferIndex != 98; ++bufferIndex) {
                // Skip the left/right borders - we want 30 characters per line rather than 32 to deal with overscan.
                if (bufferIndex == 34 || bufferIndex == 66) {
                    bufferIndex += 2;
                    // Take a short break to make sure we don't have a glitch because of showing the wrong chr bank.
                    ppu_wait_nmi();
                    wait_for_sprite0_hit();
                    set_chr_bank_0(CHR_BANK_TILES);

                }

                // Only add things to the drawing buffer if we have not hit our null terminator.
                if (haveHitNull) {
                    // Draw a space in place of anything else.
                    buffer[bufferIndex] = ' ' - TEXT_ASCII_SKIPPED_CHARACTERS;
                } else {
                    set_char_at_buffer_index();
                    if (currentChar == NULL) {
                        // Mark us as having hit the null terminator, so we stop trying to draw text.
                        haveHitNull = TRUE;
                    } else {
                        buffer[bufferIndex] = currentChar - TEXT_ASCII_SKIPPED_CHARACTERS;
                    }

                }
                ++stringIndex;
            }
            // Draw this line
            set_vram_update(buffer);
            ppu_wait_nmi();
            wait_for_sprite0_hit();
            set_chr_bank_0(CHR_BANK_TILES);
            set_vram_update(NULL);

        }

        // Each time we loop, check if A has been pressed. Set this variable if it is.
        if (pad_trigger(0) & PAD_A) {
            hasInput = TRUE;
        }

        // Draw the "more text" arrow if there is more text to go, _and_ we're not on an "off" frame
        if (!haveHitNull && (frameCount & 0x20)) {
            oam_spr(244, HUD_PIXEL_HEIGHT - 14, SPRITE_TILE_ID_TEXT_ARROW, 0x00, TEXT_ARROW_OAM_ID);
        } else {
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, SPRITE_TILE_ID_TEXT_ARROW, 0x00, TEXT_ARROW_OAM_ID);
        }
        ppu_wait_nmi();
        wait_for_sprite0_hit();
        set_chr_bank_0(CHR_BANK_TILES);

    }

    // Once we have drawn the text set this back to null, so problems are easier to find.
    currentText = NULL;

    // Clear everything back to how it was. We'll rely on update_hud to draw the health/etc back.
    for (i = 3; i != 99; ++i) {
        buffer[i] = HUD_TILE_BLANK;
    }
    set_vram_update(buffer);
    ppu_wait_nmi();
    set_vram_update(NULL);

    // Hide sprite 0 - it has now served its purpose.
    oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, HUD_SPRITE_ZERO_TILE_ID, 0x00, 0);

    // And finally put everything back how the game expects it.
    set_chr_bank_0(CHR_BANK_TILES);
    unset_nmi_chr_tile_bank();
}