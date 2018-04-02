#define PRG_BANK_MAP_SPRITES 2

#define NO_SPRITE_HIT 255

// Maximum number of keys to let the player pick up.
#define MAX_KEY_COUNT 9

// The last sprite id that collided with the player, if any. Otherwise, set to NO_SPRITE_HIT
ZEROPAGE_EXTERN(unsigned char, lastPlayerSpriteCollisionId);

// Update all sprites on the current map tile. You probably want to call this 1x/frame.
void update_map_sprites();