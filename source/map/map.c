#include "source/map/map.h"
#include "source/map/load_map.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/graphics/palettes.h"
#include "source/graphics/hud.h"

CODE_BANK(PRG_BANK_MAP_LOGIC);

ZEROPAGE_DEF(unsigned char, playerOverworldPosition);

unsigned char currentMap[256];

unsigned char assetTable[0x30];

ZEROPAGE_DEF(static int, currentValue);
ZEROPAGE_DEF(static int, currentMemoryLocation);
ZEROPAGE_DEF(static unsigned char, bufferIndex);
ZEROPAGE_DEF(static int, xScrollPosition);

void init_map() {
    // Make sure we're looking at the right sprite and chr data, not the ones for the menu.
    set_chr_bank_0(CHR_BANK_GAME);
    set_chr_bank_1(CHR_BANK_GAME+1);

    // Also set the palettes to the in-game palettes.
    pal_bg(mainBgPalette);
    pal_spr(mainSpritePalette);

    // Do some trickery to make the HUD show up at the top of the screen, with the map slightly below.
    scroll(0, 240-HUD_PIXEL_HEIGHT);
    set_mirroring(MIRROR_MODE_VERTICAL);
}

void draw_current_map_to_nametable(int nametableAdr) {

    // Prepare to draw on the first nametable
    vram_inc(0);
    set_vram_update(NULL);

    // Loop over assetTable to clear it out. 
    for (i = 0; i != sizeof(assetTable); ++i) {
        assetTable[i] = 0x00;
    }
    j = -1;
    for (i = 0; i != 192; ++i) {
         // The top 2 bytes of map data are palette data. Skip that for now.
        currentValue = currentMap[i] & 0x3f;
        currentValue = (((currentValue >> 3)) << 5) + ((currentValue % 8) << 1);

        if (bufferIndex == 0) {
            currentMemoryLocation = nametableAdr +  ((i / 16) << 6) + ((i % 16) << 1);
        }

        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + (bufferIndex<<1)] = currentValue;
        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + (bufferIndex<<1) + 1] = currentValue + 1;
        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + (bufferIndex<<1) + 32] = currentValue + 16;
        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + (bufferIndex<<1) + 33] = currentValue + 17;


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
        assetTable[j] += currentValue;

        // Every 16 frames, write the buffered data to the screen and start anew.
        ++bufferIndex;
        if (bufferIndex == 16) {
            bufferIndex = 0;
            // Bunch of messy-looking stuff that tells neslib where to write this to the nametable, and how.
            screenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
            screenBuffer[1] = LSB(currentMemoryLocation);
            screenBuffer[2] = 65;
            screenBuffer[64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
            set_vram_update(screenBuffer);
            ppu_wait_nmi();
            if (xScrollPosition != 0) {
                scroll(0, 240 - HUD_PIXEL_HEIGHT);
                split(xScrollPosition, 0);
            }

        }
    }
    // Draw the palette that we built up above.
    // Start by copying it into screenBuffer, so we can tell neslib where this lives.
    for (i = 0; i != 0x30; ++i) {
        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + i] = assetTable[i];
    }
    screenBuffer[0] = MSB(nametableAdr + 0x3c0) | NT_UPD_HORZ;
    screenBuffer[1] = LSB(nametableAdr + 0x3c0);
    screenBuffer[2] = 0x30;
    screenBuffer[0x33] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    ppu_wait_nmi();
    if (xScrollPosition != 0) {
        scroll(0, 240 - HUD_PIXEL_HEIGHT);
        split(xScrollPosition, 0);
    }

    set_vram_update(NULL);
    
}

void draw_current_map_to_a() {
    draw_current_map_to_nametable(NAMETABLE_A);
}

void draw_current_map_to_b() {
    draw_current_map_to_nametable(NAMETABLE_B);
}

void draw_current_map_to_c() {
    draw_current_map_to_nametable(NAMETABLE_C);
}


void do_screen_scroll() {
    // First, draw the next tile onto b
    // FIXME: Which direction are we going?
    load_map();
    xScrollPosition = 0;
    draw_current_map_to_nametable(NAMETABLE_B);
    
    // Draw a sprite into 0 to give us something to split on
    oam_spr(247, HUD_PIXEL_HEIGHT-NES_SPRITE_HEIGHT-2, HUD_SPRITE_ZERO_TILE_ID, 0x20, 0);

    // TODO: Move and animate player sprite somehow
    for (i = 0; i != 254; i+=2) {
        scroll(0, 240 - HUD_PIXEL_HEIGHT);
        split(i, 0);
        ppu_wait_nmi();
    }

    // Fun fact; the scroll routine actually handles nametables for you; hence the parameters being 16 bit instead of 8!
    // scroll(256, 240 - HUD_PIXEL_HEIGHT);
    xScrollPosition = 256;

    // Now, draw back to our original nametable...
    draw_current_map_to_nametable(NAMETABLE_A);

    // and bump the player back to the first screen now that we're done.
    scroll(0, 240 - HUD_PIXEL_HEIGHT);



    gameState = GAME_STATE_RUNNING;

}