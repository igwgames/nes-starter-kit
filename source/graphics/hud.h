// Some defines for the elements in the HUD
#define PRG_BANK_HUD 2

#define HUD_POSITION_START 0x0300
#define HUD_HEART_START 0x0361
#define HUD_KEY_START 0x037d
#define HUD_ATTRS_START 0x03f0

#define HUD_TILE_HEART 0xe7
#define HUD_TILE_HEART_EMPTY 0xe9
#define HUD_TILE_KEY 0xe8
#define HUD_TILE_NUMBER 0xf6
#define HUD_TILE_BLANK 0xf2
#define HUD_TILE_BORDER_BL 0xf4
#define HUD_TILE_BORDER_BR 0xf5
#define HUD_TILE_BORDER_HORIZONTAL 0xe5
#define HUD_TILE_BORDER_VERTICAL 0xe4

#define HUD_SPRITE_ZERO_TILE_ID 0xfb

// Draw the HUD
void draw_hud();

// Update the number of hearts, coins, etc in the hud.
void update_hud();
