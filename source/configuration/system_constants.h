// Width/height of the screen by console 8x8 tiles
#define SCREEN_WIDTH_TILES 32
#define SCREEN_HEIGHT_TILES 30

// Width/height of the screen by our 16x16 map tiles
#define SCREEN_WIDTH_MAP_TILES 16
#define SCREEN_HEIGHT_MAP_TILES 15

// The size of rooms stored in the map. The rest of the height is used for the HUD.
#define ROOM_WIDTH_MAP_TILES 16
#define ROOM_HEIGHT_MAP_TILES 12

#define CHR_BANK_MENU 0
#define CHR_BANK_GAME 2

#define HUD_TILE_SIZE 192
#define HUD_PIXEL_HEIGHT 48

#define MIRROR_MODE_VERTICAL 2
#define MIRROR_MODE_HORIZONTAL 3

#define NES_SPRITE_WIDTH 8
#define NES_SPRITE_HEIGHT 8

// Sprite direction definitions
// NOTE: These values are specifically chosen such that if you add direction
// to the top-left sprite in the "down" animation, you'll get the first animation 
// for that direction. It simplifies some logic.
#define SPRITE_DIRECTION_STATIONARY 0x02
#define SPRITE_DIRECTION_LEFT 0x24
#define SPRITE_DIRECTION_RIGHT 0x20
#define SPRITE_DIRECTION_UP 0x04
#define SPRITE_DIRECTION_DOWN 0x00

#define SPRITE_OFFSCREEN 0xfe

// Change this to change how fast sprites animate; values are from 0-6. 0 is fast. 6 is slow.
#define SPRITE_ANIMATION_SPEED_DIVISOR 3

#define SCREEN_EDGE_LEFT 8
#define SCREEN_EDGE_RIGHT 240
#define SCREEN_EDGE_TOP 8 + HUD_PIXEL_HEIGHT
#define SCREEN_EDGE_BOTTOM 220

// Data included before a nametable update if done mid-frame.
#define NAMETABLE_UPDATE_PREFIX_LENGTH 3

// Constants for songs we play in the game
#define SONG_TITLE 0
#define SONG_OVERWORLD 1

#define ATTRIBUTE_TABLE_OFFSET 0x3c0
#define NAMETABLE_A_ATTRS NAMETABLE_A + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_B_ATTRS NAMETABLE_B + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_C_ATTRS NAMETABLE_C + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_D_ATTRS NAMETABLE_D + ATTRIBUTE_TABLE_OFFSET