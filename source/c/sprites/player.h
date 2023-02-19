#include "source/c/library/bank_helpers.h"
#define PRG_BANK_PLAYER_SPRITE 2

// Top-left tile of the sprite of the player facing downward.
#define PLAYER_SPRITE_TILE_ID 0x00

// The id of the first sprite in the 4 sprites comprising the player. (out of 64 total sprites)
#define PLAYER_SPRITE_INDEX 0x10

// This is simple movement style that leaves everything up to the player.
#define MOVEMENT_STYLE_LOOSE 0
// This is a movement style that guides the player onto an 8px "grid", to make it harder to get
// stuck on edges. 
#define MOVEMENT_STYLE_GRID 1

// This define changes how the player moves to be one of the two movement styles above.
#define PLAYER_MOVEMENT_STYLE MOVEMENT_STYLE_GRID

// This is the width and height of player as used in collisions. This is shifted by 4 to allow for sub-pixel
// sizes. (And smoother acceleration/deceleration.) A 16px wide sprite is 256 units here.
#define PLAYER_WIDTH_EXTENDED 205
#define PLAYER_HEIGHT_EXTENDED 120
#define PLAYER_X_OFFSET_EXTENDED 28
#define PLAYER_Y_OFFSET_EXTENDED 136

// We store extra detail about the player's position to make movement smoother. This is how much to shift
// by to get to the real location. (Think about this as dividing by 2^(this number))
#define PLAYER_POSITION_SHIFT 4

// This is used to help nudge the player when their velocity is 0 in a dimension.
#define PLAYER_POSITION_TILE_MASK 0x0070
// This is used to figure out where the middle of the grid is. It's half of the max value of the mask above.
#define PLAYER_POSITION_TILE_MASK_MIDDLE 0x0040
// On the Y axis, due to some oddities in the engine, we have to nudge things over by 1 pixel
#define PLAYER_POSITION_TILE_MASK_EXTRA_NUDGE 0x10

// Handles player movement - we start slow and accelerate/decelerate up to a max speed. As a player,
// you probably won't notice this outright, but it feels a lot more natural like this.
// NOTE: The max velocity needs to be evenly divided by the acceleration. (Eg 20/4 = 5)
#define PLAYER_MAX_VELOCITY 20 // Max speed of the player; we won't let you go past this.
#define PLAYER_VELOCITY_ACCEL 4 // How quickly do we get up to max velocity?
#define PLAYER_VELOCITY_NUDGE 8 // How quickly do we "nudge" the player onto the grid? THIS MUST BE A MULTIPLE OF THE ABOVE NUMBER

// If the player is damaged, how long do we lock their controls such that they bounce backwards?
#define PLAYER_DAMAGE_CONTROL_LOCK_TIME 20
// If the player is damaged, how long are they immune to damage?
#define PLAYER_DAMAGE_INVULNERABILITY_TIME 30
// If the player is invulnerable, how quickly to we blink the sprite? In sprite mask form, so one of 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
#define PLAYER_INVULNERABILITY_BLINK_MASK 0x02

ZEROPAGE_EXTERN(int, playerXPosition);
ZEROPAGE_EXTERN(int, playerYPosition);
ZEROPAGE_EXTERN(int, playerXVelocity);
ZEROPAGE_EXTERN(int, playerYVelocity);
ZEROPAGE_EXTERN(int, nextPlayerXPosition);
ZEROPAGE_EXTERN(int, nextPlayerYPosition);
ZEROPAGE_EXTERN(unsigned char, playerDirection);

// Helper macro to convert the player's X and Y position into a position on the map
#define PLAYER_MAP_POSITION(xPos, yPos) (xPos>>4) + (yPos & 0xf0)

// Move variables for the player around, 
// and otherwise deal with controller input. (NOTE: Pause/etc are handled here too)
void prepare_player_movement(void);

// Take the updated variables and re-apply them to the player, after any adjustments are made.
void do_player_movement(void);

// Update the player's sprite, and put it onto the screen as necessary
void update_player_sprite(void);

// Tests if the player is about to collide with any solid tiles, and adjusts the player's velocity to zero if found.
void test_player_tile_collision(void);

// The sprite update method gives us an id of a sprite that we most recently hit. This reacts to that appropriately.
void handle_player_sprite_collision(void);