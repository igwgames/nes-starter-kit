#include "source/library/bank_helpers.h"

#define PRG_BANK_MAP_LOGIC 2

// We need to push the player a little bit over each frame when we do a screen scroll, or they will stay offscreen 
// when the scroll is done, and trigger a new one. Plus it looks kinda crummy if they stay put. This is added to the
// player's position with each frame.
#define SCREEN_SCROLL_NUDGE 3

// This defines how many pixels to move the screen during each frame of the screen scroll animation.
// Must be a multiple of 2 (or w`hatever value is set below)
// 2 = slow, 4 = fast, 8 = very fast, 16 = almost instant
// FIXME: Speeds faster than 2 are hyper glitchy going up/down right now...
#define SCREEN_SCROLL_SPEED 4
// This is how many pixels are done in each loop - the variable above controls when we wait for interrupts. 
// You probably don't want to change this...
#define SCREEN_SCROLL_LOOP_INCREMENT 2

// This controls how much ram we reserve for sprite data lookups. This sets us up to have 16 byte per sprite,
// but if you change the code around a bit, you might be able to drop this to 3 to use 8 bytes per sprite instead.
#define MAP_SPRITE_DATA_SHIFT 4

// Max number of sprites to load from a map tile. Note that this is also coded into the conversion tool that
// nes-starter-kit uses. Also note that there isn't much room for more... unless you really know the NES hardware
// intricately, you probably don't want to touch this one.
#define MAP_MAX_SPRITES 12

// Lookup positions for sprite data in currentMapSpriteData.
#define MAP_SPRITE_DATA_POS_X                   0
#define MAP_SPRITE_DATA_POS_Y                   2
#define MAP_SPRITE_DATA_POS_TYPE                4
#define MAP_SPRITE_DATA_POS_SIZE_PALETTE        5
#define MAP_SPRITE_DATA_POS_ANIMATION_TYPE      6
#define MAP_SPRITE_DATA_POS_HEALTH              7
#define MAP_SPRITE_DATA_POS_TILE_ID             8

// How many tiles are in the map before we start getting into sprite data.
#define MAP_DATA_TILE_LENGTH 192

// The current map; usable for collisions/etc
extern unsigned char currentMap[256];

// Supporting data for sprites; 16 bytes per sprite. Look at the sprite loader function in `map.h` (or the guide) for more details.
extern unsigned char currentMapSpriteData[192];

// The player's position on the world map. 0-7 are first row, 8-15 are 2nd, etc...
ZEROPAGE_EXTERN(unsigned char, playerOverworldPosition);

// Load the sprites from the current map into currentMapSpriteData, and the sprite list.
void load_sprites();

// Set some default variables and hardware settings to prepare to draw the map after showing menus/etc.
void init_map();

// Assuming the map is available in currentMap, draw it to a nametable. Assumes ppu is already turned off.
void draw_current_map_to_a();
void draw_current_map_to_b();
void draw_current_map_to_c();
void draw_current_map_to_d();

// Take the value of playerOverworldPosition, and transition to this with a pretty scrolling animation.
// NOTE: This is INCOMPLETE - it needs a lot of work to make it reasonable. Don't expect it to work.
// TODO: Fix this up.
void do_scroll_screen_transition();

// Take the value of playerOverworldPosition, and transition onto that screen with a fade animation.
void do_fade_screen_transition();