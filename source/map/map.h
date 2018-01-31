#include "source/library/bank_helpers.h"

#define PRG_BANK_MAP_LOGIC 2

// The current map; usable for collisions/etc
extern unsigned char currentMap[256];

// The player's position on the world map. 0-7 are first row, 8-15 are 2nd, etc...
ZEROPAGE_EXTERN(unsigned char, playerOverworldPosition);

// Assuming the map is available in currentMap, draw it. Assumes ppu is already turned off.
void draw_current_map();