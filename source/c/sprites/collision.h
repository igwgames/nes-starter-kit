#define FIRST_WALKABLE_SPRITE 0
#define LAST_WALKABLE_SPRITE 23
#define FIRST_SOLID_SPRITE 24
#define LAST_SOLID_SPRITE 47

#define PRG_BANK_SPRITE_COLLISION 2

// Test whether or not a given tile should stop player movement. 0: no collision, 1: collision
// This function has side effects for special tiles, but those side effects are ignored for non-players.
unsigned char test_collision(unsigned char tileId, unsigned char isPlayer);