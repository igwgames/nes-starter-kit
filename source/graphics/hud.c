#include "tools/neslib_famitracker/neslib.h"
#include "source/graphics/hud.h"
#include "source/globals.h"

CODE_BANK(PRG_BANK_HUD);

void draw_hud_to_nametable(unsigned int nametableAdr) {
    vram_adr(nametableAdr + HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(nametableAdr + 0x3f0);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }
}


void draw_hud() {
    // We draw the hud to both active nametables to hide glitches if the timing for ppu scrolling isn't perfect -
    // otherwise the last few pixels of the hud will start jumping around.
    draw_hud_to_nametable(NAMETABLE_A);
    draw_hud_to_nametable(NAMETABLE_D);
}

void update_hud() {
    // This sets up screenBuffer to print x hearts, then x more empty hearts. 
    // You give it the address, tell it the direction to write, then follow up with
    // Ids, ending with NT_UPD_EOF
    screenBuffer[0] = MSB(HUD_HEART_START) | NT_UPD_HORZ;
    screenBuffer[1] = LSB(HUD_HEART_START);
    screenBuffer[2] = playerMaxHealth;
    for (i = 0; i != playerHealth; ++i) {
        screenBuffer[i+3] = HUD_TILE_HEART;
    }
    for (; i != playerMaxHealth; ++i) {
        screenBuffer[i+3] = HUD_TILE_HEART_EMPTY;
    }
    screenBuffer[11] = NT_UPD_EOF;
    set_vram_update(screenBuffer);

}