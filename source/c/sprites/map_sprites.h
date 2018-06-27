#define PRG_BANK_MAP_SPRITES 2
#include "source/c/library/bank_helpers.h"

#define NO_SPRITE_HIT 255

// Maximum number of keys to let the player pick up.
#define MAX_KEY_COUNT 9

// We make our 16x16 sprites a little bit smaller than they appear, so collisions with them are
// less likely. This makes getting hit by enemies less likely. This is only for collisions with
// the player.
#define SPRITE_HITBOX_OFFSET 24
// This does the same thing, but only for sprite collisions with tiles.
#define SPRITE_TILE_HITBOX_OFFSET 10

// The number of frames to flicker the enemy sprite for (and disallow damage) before returning to normal.
#define SPRITE_INVULNERABILITY_TIME 60

// Used to figure out where to put the sprites in sprite memory. 

// Have to skip over sprite 0 (0x00) and player (0x10)
#define FIRST_ENEMY_SPRITE_OAM_INDEX 0x40

// The last sprite id that collided with the player, if any. Otherwise, set to NO_SPRITE_HIT
ZEROPAGE_EXTERN(unsigned char, lastPlayerSpriteCollisionId);
ZEROPAGE_EXTERN(unsigned char, currentMapSpriteIndex);
ZEROPAGE_EXTERN(unsigned char, lastPlayerWeaponCollisionId);

// Update all sprites on the current map tile. You probably want to call this 1x/frame.
void update_map_sprites(void);

// Little helper to turn an X,Y position to a tile on the map
#define SPRITE_MAP_POSITION(xPos, yPos) ((xPos)>>(SPRITE_POSITION_SHIFT+4)) + (((yPos >> SPRITE_POSITION_SHIFT) - HUD_PIXEL_HEIGHT) & 0xf0)
