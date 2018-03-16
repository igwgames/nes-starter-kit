#include "source/map/map.h"
#include "source/map/load_map.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/graphics/palettes.h"
#include "source/graphics/hud.h"
#include "source/graphics/fade_animation.h"
#include "source/sprites/player.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_MAP_LOGIC);

ZEROPAGE_DEF(unsigned char, playerOverworldPosition);

unsigned char currentMap[256];

unsigned char assetTable[0x38];

ZEROPAGE_DEF(static int, currentValue);
ZEROPAGE_DEF(static int, currentMemoryLocation);
ZEROPAGE_DEF(static unsigned char, bufferIndex);
ZEROPAGE_DEF(static int, xScrollPosition);
ZEROPAGE_DEF(static unsigned char, otherLoopIndex);
ZEROPAGE_DEF(static unsigned char, tempArrayIndex);

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

// Clears the asset table. Set containsHud to 1 to set the HUD bytes to use palette 4 (will break the coloring logic if you use the
// last few rows for the map.)
void clear_asset_table(containsHud) {
    // Loop over assetTable to clear it out. 
    for (i = 0; i != sizeof(assetTable) - 8; ++i) {
        assetTable[i] = 0x00;
    }
    // The last row of the asset table uses the 4th palette to show the HUD correctly.
    for (; i != sizeof(assetTable); ++i) {
        assetTable[i] = containsHud == 0 ? 0x00 : 0xff;
    }
}


// FIXME: constants for attribute table addrs
// reverseAttributes: If set to 1, this will flip which bits are used for the top and the bottom palette in the attribute table.
//                    This allows us to correctly draw starting on an odd-numbered row (such as at the start of our HUD.) 
void draw_current_map_to_nametable(int nametableAdr, int attributeTableAdr, unsigned char reverseAttributes) {

    // Prepare to draw on the first nametable
    vram_inc(0);
    set_vram_update(NULL);

    if (!reverseAttributes) {
        j = -1;
    } else {
        j = 7;
    }
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
        if (i % 32 == (reverseAttributes ? 0 : 16)) 
			j -= 8;
		if (i % 2 == 0) 
			j++;

        // Now based on where we are in the map, shift them appropriately.
        // This builds up the palette bytes - which comprise of 2 bits per 16x16 tile. It's a bit confusing...
        if (i % 2 == 0) {
			// Even/left
			if ((i / 16) % 2 == reverseAttributes) {
				// top
				currentValue >>= 6;
			} else {
				//bottom
				currentValue >>= 2;
			}
		} else {
			// Odd/right
			if ((i / 16) % 2 == reverseAttributes) {
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
        if (bufferIndex == 8) {
            if (xScrollPosition != -1) {
                scroll(0, 240 - HUD_PIXEL_HEIGHT);
                split(xScrollPosition, 0);
            }
        }
        if (bufferIndex == 16) {
            bufferIndex = 0;
            // Bunch of messy-looking stuff that tells neslib where to write this to the nametable, and how.
            screenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
            screenBuffer[1] = LSB(currentMemoryLocation);
            screenBuffer[2] = 65;
            screenBuffer[64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
            set_vram_update(screenBuffer);
            ppu_wait_nmi();
            if (xScrollPosition != -1) {
                scroll(0, 240 - HUD_PIXEL_HEIGHT);
                split(xScrollPosition, 0);
            }
            set_vram_update(NULL);

        }
    }
    // Draw the palette that we built up above.
    // Start by copying it into screenBuffer, so we can tell neslib where this lives.
    for (i = 0; i != 0x38; ++i) {
        screenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + i] = assetTable[i];
    }
    screenBuffer[0] = MSB(attributeTableAdr) | NT_UPD_HORZ;
    screenBuffer[1] = LSB(attributeTableAdr);
    screenBuffer[2] = 0x38;
    screenBuffer[0x3b] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    ppu_wait_nmi();
    if (xScrollPosition != -1) {
        scroll(0, 240 - HUD_PIXEL_HEIGHT);
        split(xScrollPosition, 0);
    }

    set_vram_update(NULL);
    
}

// Draw a row (technically two rows) of tiles onto the map. Breaks things up so we can hide
// the change behind the HUD while continuing to use vertical mirroring.
// This basically is the draw_current_map_to_nametable logic, but it stops after 32. 
// NOTE: i and j MUST be maintained between calls to this method.
void draw_individual_row(int nametableAdr, int attributeTableAdr, char oliChange) {
    while(1) {
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
        if (bufferIndex == 8) {
            ppu_wait_nmi();
            if (xScrollPosition != -1) {
                otherLoopIndex += oliChange;
                scroll(0, 240 - HUD_PIXEL_HEIGHT);
                split_y(256, 240 + 48 + otherLoopIndex);
            }
        }
        if (bufferIndex == 16) {
            bufferIndex = 0;
            // Bunch of messy-looking stuff that tells neslib where to write this to the nametable, and how.
            screenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
            screenBuffer[1] = LSB(currentMemoryLocation);
            screenBuffer[2] = 65;
            // We wrote the 64 tiles in the loop above; they're ready to go.

            // Add in another update for the palette
            tempArrayIndex = 64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1;
            screenBuffer[tempArrayIndex++] = MSB(attributeTableAdr + j - 7) | NT_UPD_HORZ;
            screenBuffer[tempArrayIndex++] = LSB(attributeTableAdr + j - 7);
            screenBuffer[tempArrayIndex++] = 8;

            // Using an unrolled loop to save a bit of RAM - not like we need it really.
            screenBuffer[tempArrayIndex++] = assetTable[j-7];
            screenBuffer[tempArrayIndex++] = assetTable[j-6];
            screenBuffer[tempArrayIndex++] = assetTable[j-5];
            screenBuffer[tempArrayIndex++] = assetTable[j-4];
            screenBuffer[tempArrayIndex++] = assetTable[j-3];
            screenBuffer[tempArrayIndex++] = assetTable[j-2];
            screenBuffer[tempArrayIndex++] = assetTable[j-1];
            screenBuffer[tempArrayIndex++] = assetTable[j];
            screenBuffer[tempArrayIndex++] = NT_UPD_EOF;

            set_vram_update(screenBuffer);
            ppu_wait_nmi();
            if (xScrollPosition != -1) {
                scroll(0, 240 - HUD_PIXEL_HEIGHT);
                split_y(256, 240 + 48 + otherLoopIndex);
            }
            set_vram_update(NULL);

        }
        ++i;
        if (i % 32 == 0) {
            break;
        }
    }

}

void draw_current_map_to_a() {
    clear_asset_table(1);
    xScrollPosition = -1;
    draw_current_map_to_nametable(NAMETABLE_A, NAMETABLE_A_ATTRS, 0);
}

void draw_current_map_to_b() {
    clear_asset_table(0);
    xScrollPosition = -1;
    draw_current_map_to_nametable(NAMETABLE_B, NAMETABLE_B_ATTRS, 0);
}

void draw_current_map_to_c() {
    clear_asset_table(0);
    xScrollPosition = -1;
    draw_current_map_to_nametable(NAMETABLE_C, NAMETABLE_C_ATTRS, 0);
}

void draw_current_map_to_d() {
    clear_asset_table(0);
    xScrollPosition = -1;
    draw_current_map_to_nametable(NAMETABLE_D, NAMETABLE_D_ATTRS, 0);
}

// A quick, low-tech glamour-free way to transition between screens.
void do_fade_screen_transition() {
    load_map();
    clear_asset_table(1);
    fade_out_fast();
    // Now that the screen is clear, migrate the player's sprite a bit..
    if (playerDirection == SPRITE_DIRECTION_LEFT) {
        playerXPosition = (SCREEN_EDGE_RIGHT << PLAYER_POSITION_SHIFT) - (SCREEN_SCROLL_NUDGE << (PLAYER_POSITION_SHIFT+1));
    } else if (playerDirection == SPRITE_DIRECTION_RIGHT) {
        playerXPosition = (SCREEN_EDGE_LEFT << PLAYER_POSITION_SHIFT) + (SCREEN_SCROLL_NUDGE << (PLAYER_POSITION_SHIFT+1));
    } else if (playerDirection == SPRITE_DIRECTION_UP) {
        playerYPosition = (SCREEN_EDGE_BOTTOM << PLAYER_POSITION_SHIFT) - (SCREEN_SCROLL_NUDGE << (PLAYER_POSITION_SHIFT+1));
    } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
        playerYPosition = (SCREEN_EDGE_TOP << PLAYER_POSITION_SHIFT) + (SCREEN_SCROLL_NUDGE << (PLAYER_POSITION_SHIFT+1));
    }
    // Actually move the sprite too, since otherwise this won't happen until after we un-blank the screen.
    banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);

    // Draw the updated map to the screen...
    draw_current_map_to_nametable(NAMETABLE_A, NAMETABLE_A_ATTRS, 0);
    fade_in_fast();
    // Aand we're back!
    gameState = GAME_STATE_RUNNING;
}

// A pretty screel scrolling method. Note: This is incomplete - up/down are slower than left/right, and up is very ugly.
// TODO: Fix this up and make it usable.
void do_scroll_screen_transition() {
    // First, draw the next tile onto b
    xScrollPosition = -1;
    scroll(0, 240 - HUD_PIXEL_HEIGHT);
    
    // Draw a sprite into 0 to give us something to split on
    oam_spr(249, HUD_PIXEL_HEIGHT-NES_SPRITE_HEIGHT-0, HUD_SPRITE_ZERO_TILE_ID, 0x00, 0);
    ppu_wait_nmi();

    if (playerDirection == SPRITE_DIRECTION_RIGHT) {
        load_map();

        clear_asset_table(1);
        draw_current_map_to_nametable(NAMETABLE_B, NAMETABLE_B_ATTRS, 0);
        for (i = 0; i != 254; i+= SCREEN_SCROLL_LOOP_INCREMENT) {
            playerXPosition -= (SCREEN_SCROLL_LOOP_INCREMENT << PLAYER_POSITION_SHIFT) - SCREEN_SCROLL_NUDGE;
            banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);
            if (i % SCREEN_SCROLL_SPEED == 0) {
                ppu_wait_nmi();
                split(i, 0);
            }
        }
        xScrollPosition = 256;
    } else if (playerDirection == SPRITE_DIRECTION_LEFT) {
        load_map();

        clear_asset_table(1);
        draw_current_map_to_nametable(NAMETABLE_B, NAMETABLE_B_ATTRS, 0);
        for (i = 0; i != 254; i+= SCREEN_SCROLL_LOOP_INCREMENT) { // we depend on i being an 8 bit integer here (values from 0-255), so 0 rolls over to 254.
            playerXPosition += (SCREEN_SCROLL_LOOP_INCREMENT << PLAYER_POSITION_SHIFT) - SCREEN_SCROLL_NUDGE;
            banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);
            if (i % SCREEN_SCROLL_SPEED == 0) {
                ppu_wait_nmi();
                split(512-i, 0);
            }
        }
        xScrollPosition = 256;
    } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
        // First draw original map to the other nametable
        clear_asset_table(0);
        draw_current_map_to_nametable(NAMETABLE_B + (SCREEN_WIDTH_TILES*6), NAMETABLE_B_ATTRS + 8, 1);
        
        load_map();
        // Loop over the screen, drawing the map in the space taken up by the hud every time we go 32 lines (2 tiles)
        // NOTE: We use both i and j in the loop inside one of the functions we're calling, so we needed another variable.
        clear_asset_table(0);
        i = 0; 
        j = -1;
        xScrollPosition = 256;
        for (otherLoopIndex = 0; otherLoopIndex < 240 - HUD_PIXEL_HEIGHT; otherLoopIndex += SCREEN_SCROLL_LOOP_INCREMENT) {
            // Going down needs a little bit less of a nudge for some reason - suspect it has something to do with the HUD.
            // If you have an explanation for this, please file a ticket and let me know how to update this!
            playerYPosition -= (SCREEN_SCROLL_LOOP_INCREMENT << PLAYER_POSITION_SHIFT) - (SCREEN_SCROLL_NUDGE>>1);
            banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);
            if (otherLoopIndex % 32 == 0 && otherLoopIndex < 224) {
                ppu_wait_nmi();
                split_y(256, 240 + 48 + otherLoopIndex);

                draw_individual_row(NAMETABLE_B, NAMETABLE_B_ATTRS, SCREEN_SCROLL_LOOP_INCREMENT);
            } else {
                if ((i % (SCREEN_SCROLL_SPEED*4)) == 0) {
                    ppu_wait_nmi();
                    split_y(256, 240 + 48 + otherLoopIndex);
                }
            }
        }

        xScrollPosition = 256;
    } else if (playerDirection == SPRITE_DIRECTION_UP) {
        // First draw original map to the other nametable
        clear_asset_table(0);
        draw_current_map_to_nametable(NAMETABLE_B + (SCREEN_WIDTH_TILES*6), NAMETABLE_B_ATTRS + 8, 1);
        load_map();
        // Loop over the screen, drawing the map in the space taken up by the hud every time we go 32 lines (2 tiles)
        // NOTE: We use both i and j in the loop inside one of the functions we're calling, so we needed another variable.
        clear_asset_table(0);
        i = 0; 
        j = -1;
        xScrollPosition = 256;
        // NOTE: For the case here, we test against < 242, because all valid scroll positions are below 242. 
        // Since we're using an unsigned char, 0-1 = 255, so as soon as we get below zero the loop terminates.
        for (otherLoopIndex = 242 - HUD_PIXEL_HEIGHT; otherLoopIndex < 242; otherLoopIndex -= SCREEN_SCROLL_LOOP_INCREMENT) {
            playerYPosition += (SCREEN_SCROLL_LOOP_INCREMENT << PLAYER_POSITION_SHIFT) - SCREEN_SCROLL_NUDGE;
            banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);
            if (otherLoopIndex % 32 == 0 && otherLoopIndex != 0) {
                // TODO: Need to figure out how to make this work in reverse order. (Mess with i and j, I assume)
                ppu_wait_nmi();
                split_y(256, 240 + 48 + otherLoopIndex);

                draw_individual_row(NAMETABLE_B, NAMETABLE_B_ATTRS, -SCREEN_SCROLL_LOOP_INCREMENT);
            } else {
                if (i % (SCREEN_SCROLL_SPEED<<1) == 0) {
                    ppu_wait_nmi();
                    split_y(256, 240 + 48 + otherLoopIndex);
                }
            }
        }

        xScrollPosition = 256;

    }

    // Now, draw back to our original nametable...
    clear_asset_table(1);
    draw_current_map_to_nametable(NAMETABLE_A, NAMETABLE_A_ATTRS, 0);

    // and bump the player back to the first screen now that we're done.
    scroll(0, 240 - HUD_PIXEL_HEIGHT);

    // Hide sprite 0 - it has now served its purpose.
    oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, HUD_SPRITE_ZERO_TILE_ID, 0x00, 0);

    xScrollPosition = -1;
    gameState = GAME_STATE_RUNNING;

}