// We combine size and palette into one byte for sprites, since neither one needs a full byte. (Palette needs 2 bits!)
// The first 2 bits are used for the palette, and the rest used for the size.
// As a result, you have to use a mask to get either value - for example to get size, you have to do `theValue & SPRITE_SIZE_MASK`,
// or to get the palette, you can use `theValue & SPRITE_PALETTE_MASK`
#define SPRITE_SIZE_MASK 0x3f
#define SPRITE_SIZE_8PX_8PX 0x00
#define SPRITE_SIZE_16PX_16PX 0x01

#define SPRITE_PALETTE_MASK 0xc0
#define SPRITE_PALETTE_0 0x00
#define SPRITE_PALETTE_1 0x40
#define SPRITE_PALETTE_2 0x80
#define SPRITE_PALETTE_3 0xc0

#define SPRITE_ANIMATION_NONE 0
#define SPRITE_ANIMATION_SWAP 10
#define SPRITE_ANIMATION_SWAP_FAST 11
#define SPRITE_ANIMATION_FULL 50

// Use _NOTHING for decoration sprites that don't do anything when collided with.
#define SPRITE_TYPE_NOTHING 0x00
#define SPRITE_TYPE_KEY 0x01
#define SPRITE_TYPE_HEALTH 0x02
// Used _STATIC_ENEMY for sprites that deal damage, but otherwise don't move.
#define SPRITE_TYPE_STATIC_ENEMY 0x03 

extern const unsigned char sprite_definitions[];