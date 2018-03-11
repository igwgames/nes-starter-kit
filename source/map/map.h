#include "source/library/bank_helpers.h"

#define PRG_BANK_MAP_LOGIC 2

// We need to push the player a little bit over each frame when we do a screen scroll, or they will stay offscreen 
// when the scroll is done, and trigger a new one. Plus it looks kinda crummy if they stay put. This is added to the
// player's position with each frame.
#define SCREEN_SCROLL_NUDGE 3

// This defines how many pixels to move the screen during each frame of the screen scroll animation.
// Must be a multiple of 2 (or whatever value is set below)
// 2 = slow, 4 = fast, 8 = very fast, 16 = almost instant
// FIXME: Speeds faster than 2 are hyper glitchy going up/down right now...
#define SCREEN_SCROLL_SPEED 2
// This is how many pixels are done in each loop - the variable above controls when we wait for interrupts. 
// You probably don't want to change this...
#define SCREEN_SCROLL_LOOP_INCREMENT 2

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