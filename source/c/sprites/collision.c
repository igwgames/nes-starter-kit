#include "source/c/sprites/collision.h"
#include "source/c/library/bank_helpers.h"

CODE_BANK(PRG_BANK_SPRITE_COLLISION);
ZEROPAGE_DEF(unsigned char, collisionTemp);

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer) {
    // The top 2 bits of the tile id are the palette color. We don't care about that here, so skip them.
    collisionTemp = tileId & 0x3f;
    if (collisionTemp >= FIRST_SOLID_SPRITE && collisionTemp <= LAST_SOLID_SPRITE) {
        return 1;
    }
    return 0;
}