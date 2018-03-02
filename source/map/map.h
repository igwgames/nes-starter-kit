#include "source/library/bank_helpers.h"

#define PRG_BANK_MAP_LOGIC 2

// The current map; usable for collisions/etc
extern unsigned char currentMap[256];

// The player's position on the world map. 0-7 are first row, 8-15 are 2nd, etc...
ZEROPAGE_EXTERN(unsigned char, playerOverworldPosition);

// Set some default variables and hardware settings to prepare to draw the map after showing menus/etc.
void init_map();

// Assuming the map is available in currentMap, draw it to a nametable. Assumes ppu is already turned off.
void draw_current_map_to_a();
void draw_current_map_to_b();
void draw_current_map_to_c();
void draw_current_map_to_d();

// Take the value of playerOverworldPosition, draw the new map on the 2nd or 3rd nametable, then orchestrate a scroll.
void do_screen_scroll();