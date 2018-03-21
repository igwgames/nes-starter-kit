#include "source/sprites/sprite_definitions.h"

// TODO: Is it worth putting this in a bank, or is it ok use of the kernel? Leaning towards the latter.

// Put all newly-designed sprites here. 8 Bytes per sprite, defined mostly from constants
// in sprite_definitions.h. The 8 bytes are: 
// 1st byte: Sprite type 
// 2nd byte: Tile id for first tile, using 8x8 grid. See the guide for more detail on this.
// 3rd byte: Split; contains sprite size and palette. Combine the constants
//           using the logical OR operator (represented by the bar character: |) 
// 4th byte: Animation type
// 5th-8th byte: Miscellaneous; you can use this for whatever extra data you need. For enemies, byte 5
//           is used as the damage they deal. Health powerups use byte 5 to store how much to restore. 
// NOTE: This array cannot contain more than 64 sprites, or other logic will break.
const unsigned char sprite_definitions[] = {
    SPRITE_TYPE_HEALTH, 0xea, SPRITE_SIZE_8PX_8PX | SPRITE_PALETTE_0, SPRITE_ANIMATION_NONE, 0x01, 0x00, 0x00, 0x00
};