#include "source/map/tile_animation.h"
#include "source/globals.h"
#include "source/library/bank_helpers.h"

CODE_BANK(PRG_BANK_MAP_TILE_ANIMATION);

void animate_map_tiles() {
    // Using any number that equates to a single binary digit (1, 2, 4, 8, 16, 32, 64, 128) should result in a constant animation.
    if (frameCount & 0x20) {
        set_chr_bank_0(CHR_ANIMATION_BANK_0);
    } else {
        set_chr_bank_0(CHR_ANIMATION_BANK_1);
    }
}