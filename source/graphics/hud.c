#include "tools/neslib_famitracker/neslib.h"
#include "source/graphics/hud.h"
#include "source/globals.h"

CODE_BANK(PRG_BANK_HUD);

void draw_hud() {
    vram_adr(HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(0x23f0);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }
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