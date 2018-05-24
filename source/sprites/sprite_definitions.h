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

// Use this for wprites that are not animated at all.
#define SPRITE_ANIMATION_NONE 0
// This is used for sprites with two animation states; it will flip between the two.
#define SPRITE_ANIMATION_SWAP 10
// This flips sprites between two animation states, like the one above. Just faster!
#define SPRITE_ANIMATION_SWAP_FAST 11
// This is the "full" animation style - it flips between two animation frames in each direction.
#define SPRITE_ANIMATION_FULL 50

// Use this for sprites that never move.
#define SPRITE_MOVEMENT_NONE 0
// This is used for sprites that move left to right (and back) in a predictable pattern
#define SPRITE_MOVEMENT_LEFT_RIGHT 1
// This is used for sprites that move top to bottom (and back) in a predictable pattern
#define SPRITE_MOVEMENT_UP_DOWN 2
// This is used for sprites that wander in a randomized pattern, with collisions.
#define SPRITE_MOVEMENT_RANDOM_WANDER 3

// Use _NOTHING for decoration sprites that don't do anything when collided with.
// This can be used for decorations, etc...
#define SPRITE_TYPE_NOTHING 0x00
#define SPRITE_TYPE_KEY 0x01
#define SPRITE_TYPE_HEALTH 0x02
#define SPRITE_TYPE_REGULAR_ENEMY 0x03 
#define SPRITE_TYPE_DOOR 0x04
#define SPRITE_TYPE_LOCKED_DOOR 0x05
#define SPRITE_TYPE_ENDGAME 0x06

// Use this when a sprite is destroyed - this will skip all logic about the sprite.
#define SPRITE_TYPE_OFFSCREEN 0x7f

// You can use this for a tile id if you *don't* want to skip all logic.
#define SPRITE_TILE_ID_OFFSCREEN 0xee

// Used to figure out where to put the sprites in sprite memory. 
// Have to skip over sprite 0 (0x00) and player (0x10)
#define FIRST_ENEMY_SPRITE_OAM_INDEX 0x20

// How much to shift to get the position on spriteDefinitions. We store 8 bytes, so we shift by 3. 
// If you want to expand spriteDefinitions to 16 bytes per definition, change this to 4.
#define SPRITE_DEF_SHIFT 3

// How much to shift sprite positions to put them on the map. 
#define SPRITE_POSITION_SHIFT 4

// List of data contained in the sprite definitions array.
#define SPRITE_DEF_POSITION_TYPE            0
#define SPRITE_DEF_POSITION_TILE_ID         1
#define SPRITE_DEF_POSITION_SIZE_PALETTE    2
#define SPRITE_DEF_POSITION_ANIMATION_TYPE  3
#define SPRITE_DEF_POSITION_MOVEMENT_TYPE   4
#define SPRITE_DEF_POSITION_HP_RESTORE      5
#define SPRITE_DEF_POSITION_HEALTH          5
#define SPRITE_DEF_POSITION_SLIDE_SPEED     5
#define SPRITE_DEF_POSITION_MOVE_SPEED      6
#define SPRITE_DEF_POSITION_DAMAGE          7

extern const unsigned char spriteDefinitions[];

