#include "source/map/map.h"
#include "tools/neslib_famitracker/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/globals.h"
#include "source/configuration/common_variables.h"
#include "source/configuration/system_constants.h"
#include "source/graphics/palettes.h"

CODE_BANK(PRG_BANK_MAP_LOGIC);

ZEROPAGE_DEF(unsigned char, playerOverworldPosition);
ZEROPAGE_ARRAY_DEF(unsigned char, screenBuffer, 0x30);

unsigned char currentMap[256];

ZEROPAGE_DEF(static int, currentValue);
ZEROPAGE_DEF(static int, currentMemoryLocation);

void draw_current_map() {

    // Make sure we're looking at the right sprite and chr data, not the ones for the menu.
    set_chr_bank_0(CHR_BANK_GAME);
    set_chr_bank_1(CHR_BANK_GAME+1);

    // Also set the palettes to the in-game palettes.
    pal_bg(mainBgPalette);
    pal_spr(mainSpritePalette);

    // Do some trickery to make the HUD show up at the top of the screen, with the map slightly below.
    scroll(0, 240-HUD_PIXEL_HEIGHT);
    set_mirroring(MIRROR_MODE_VERTICAL);

    // Prepare to draw on the first nametable
    vram_inc(0);
    set_vram_update(NULL);

    // Loop over screenBuffer to clear it out. 
    for (i = 0; i != sizeof(screenBuffer); ++i) {
        screenBuffer[i] = 0;
    }
    j = -1;
    for (i = 0; i != 192; ++i) {
         // The top 2 bytes of map data are palette data. Skip that for now.
        currentValue = currentMap[i] & 0x3f;
        currentValue = (((currentValue >> 3)) << 5) + ((currentValue % 8) << 1);

        currentMemoryLocation = ((i / 16) << 6) + ((i % 16) << 1);

        // This isn't the most efficient way to get the map loaded, but it works and isn't painfully slow.
        vram_adr(NAMETABLE_A+currentMemoryLocation);
        vram_put(currentValue);
        vram_put(currentValue+1);

        // Skips to next row, to draw the bottom half of the tiles.
        vram_adr(NAMETABLE_A+currentMemoryLocation + 32); 
        vram_put(currentValue + 16);
        vram_put(currentValue + 17);

        // okay, now we have to update the byte for palettes. This is going to look a bit messy...
        // Start with the top 2 bytes
        currentValue = currentMap[i] & 0xc0;

        // Update where we are going to update with the palette data, which we store in the buffer.
        if (i % 32 == 16) 
			j -= 8;
		if (i % 2 == 0) 
			j++;

        // Now based on where we are in the map, shift them appropriately.
        // This builds up the palette bytes - which comprise of 2 bits per 16x16 tile. It's a bit confusing...
        if (i % 2 == 0) {
			// Even/left
			if ((i / 16) % 2 == 0) {
				// top
				currentValue >>= 6;
			} else {
				//bottom
				currentValue >>= 2;
			}
		} else {
			// Odd/right
			if ((i / 16) % 2 == 0) {
				// Top
				currentValue >>= 4;
			} else {
				// Bottom 
				currentValue >>= 0;
			}
		}
        screenBuffer[j] += currentValue;
    }
    // Draw the palette that we built up above.
    vram_adr(NAMETABLE_A + 0x3c0);
    vram_write(screenBuffer, 0x30);
    
    // Map drawing is complete; let the player play the game!
    gameState = GAME_STATE_RUNNING;
}