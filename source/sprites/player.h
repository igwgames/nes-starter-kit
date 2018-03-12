#include "source/library/bank_helpers.h"
#define PRG_BANK_PLAYER_SPRITE 2

// Top-left tile of the sprite of the player facing downward.
#define PLAYER_SPRITE_TILE_ID 0x00

// The id of the first sprite in the 4 sprites comprising the player. (out of 64 total sprites)
#define PLAYER_SPRITE_INDEX 0x10

// This is the width and height of player as used in collisions. This is shifty by 4 to allow for sub-pixel
// sizes. (And smoother acceleration/deceleration.) A 16px wide sprite is 256 units here.
#define PLAYER_WIDTH_EXTENDED 205
#define PLAYER_HEIGHT_EXTENDED 240
#define PLAYER_X_OFFSET_EXTENDED 28
#define PLAYER_Y_OFFSET_EXTENDED 16

// We store extra detail about the player's position to make movement smoother. This is how much to shift
// by to get to the real location. (Think about this as dividing by 2^(this number))
#define PLAYER_POSITION_SHIFT 4

// Handles player movement - we start slow and accelerate/decelerate up to a max speed. As a player,
// you probably won't notice this outright, but it feels a lot more natural like this.
// NOTE: The max velocity needs to be evenly divided by the acceleration. (Eg 20/4 = 5)
#define PLAYER_MAX_VELOCITY 20 // Max speed of the player; we won't let you go past this.
#define PLAYER_VELOCITY_ACCEL 4 // How quickly do we get up to max velocity? 

ZEROPAGE_EXTERN(int, playerXPosition);
ZEROPAGE_EXTERN(int, playerYPosition);
ZEROPAGE_EXTERN(int, playerXVelocity);
ZEROPAGE_EXTERN(int, playerYVelocity);
ZEROPAGE_EXTERN(unsigned char, playerDirection);

// Helper macro to convert the player's X and Y position into a position on the map
#define PLAYER_MAP_POSITION(xPos, yPos) (xPos>>4) + (yPos & 0xf0)

// Move the player around, and otherwise deal with controller input. (NOTE: Pause/etc are handled here too)
void handle_player_movement();

// Update the player's sprite, and put it onto the screen as necessary
void update_player_sprite();

// Tests if the player is about to collide with any solid tiles, and adjusts the player's velocity to zero if found.
void test_player_tile_collision();