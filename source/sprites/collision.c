#include "source/sprites/collision.h"
#include "source/library/bank_helpers.h"

CODE_BANK(PRG_BANK_SPRITE_COLLISION);

unsigned char test_collision(unsigned char tileId) {
    if (tileId >= FIRST_SOLID_SPRITE && tileId <= LAST_SOLID_SPRITE) {
        return 1;
    }
    return 0;
}