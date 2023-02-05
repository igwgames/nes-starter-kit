// Width/height of the screen by console 8x8 tiles
#define SCREEN_WIDTH_TILES 32
#define SCREEN_HEIGHT_TILES 30

// Width/height of the screen by our 16x16 map tiles
#define SCREEN_WIDTH_MAP_TILES 16
#define SCREEN_HEIGHT_MAP_TILES 15

#define CHR_ROW_WIDTH_TILES 16

// The size of rooms stored in the map. The rest of the height is used for the HUD.
#define ROOM_WIDTH_MAP_TILES 16
#define ROOM_HEIGHT_MAP_TILES 12

#define CHR_BANK_MENU 0
#define CHR_BANK_TILES 1
#define CHR_BANK_SPRITES 2

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

#define SCREEN_EDGE_LEFT 2
#define SCREEN_EDGE_RIGHT 240
#define SCREEN_EDGE_TOP HUD_PIXEL_HEIGHT-3
#define SCREEN_EDGE_BOTTOM 224

// Special case: For collisions we actually have to let the player go past the edge of the screen,
// so we wrap this to the very far right edge of the screen, making collisions simpler.
#define SCREEN_EDGE_LEFT_WRAPPED 255

// Data included before a nametable update if done mid-frame.
#define NAMETABLE_UPDATE_PREFIX_LENGTH 3

// Constants for songs we play in the game
#define SONG_TITLE 0
#define SONG_OVERWORLD 1

// Constants for sound effects we play in the game
#define SFX_HURT 0
#define SFX_HEART 1
#define SFX_KEY 2
#define SFX_GAMEOVER 3
#define SFX_WIN 4

// Constants for sfx channels. Completely unnecessary but it's easier to read
#define SFX_CHANNEL_1 0
#define SFX_CHANNEL_2 1
#define SFX_CHANNEL_3 2
#define SFX_CHANNEL_4 3

#define ATTRIBUTE_TABLE_OFFSET 0x3c0
#define NAMETABLE_A_ATTRS NAMETABLE_A + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_B_ATTRS NAMETABLE_B + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_C_ATTRS NAMETABLE_C + ATTRIBUTE_TABLE_OFFSET
#define NAMETABLE_D_ATTRS NAMETABLE_D + ATTRIBUTE_TABLE_OFFSET

#define OAM_ADDR 0x0200

#define PPU_PATTERN_TABLE_0_ADDRESS 0x0000
#define PPU_PATTERN_TABLE_1_ADDRESS 0x1000
#define PPU_PATTERN_TABLE_LENGTH 0x1000