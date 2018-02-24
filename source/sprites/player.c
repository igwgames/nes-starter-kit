#include "tools/neslib_famitracker/neslib.h"
#include "source/sprites/player.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/collision.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);
ZEROPAGE_DEF(unsigned char, playerVelocityLockTime);
ZEROPAGE_DEF(unsigned char, playerDirection);

ZEROPAGE_DEF(unsigned char, rawXPosition);
ZEROPAGE_DEF(unsigned char, rawYPosition);
ZEROPAGE_DEF(unsigned char, rawTileId);

ZEROPAGE_DEF(unsigned char, collisionTempX);
ZEROPAGE_DEF(unsigned char, collisionTempY);
ZEROPAGE_DEF(unsigned char, collisionTempXRight);
ZEROPAGE_DEF(unsigned char, collisionTempYBottom);

void update_player_sprite() {
    // Calculate the position of the player itself, then use these variables to build it up with 4 8x8 NES sprites.
    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
    rawTileId = PLAYER_SPRITE_TILE_ID + playerDirection;

    if (playerXVelocity != 0 || playerYVelocity != 0) {
        // Does some math with the current NES frame to add either 2 or 0 to the tile id, animating the sprite.
        rawTileId += ((frameCount >> SPRITE_ANIMATION_SPEED_DIVISOR) & 0x01) << 1;
    }
    
    oam_spr(rawXPosition, rawYPosition, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
    oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);

}

void handle_player_movement() {
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }

    // FIXME: Do nicer calculations with velocity and acceleration, like in ld38/etc
    playerXVelocity = 0;
    if (controllerState & PAD_LEFT) {
        playerXVelocity = -16;
    } else if (controllerState & PAD_RIGHT) {
        playerXVelocity = 16;
    } else {
        playerXVelocity = 0;
    }

    playerYVelocity = 1;
    if (controllerState & PAD_UP) {
        playerYVelocity = -16;
    } else if (controllerState & PAD_DOWN) {
        playerYVelocity = 16;
    } else { 
        playerYVelocity = 0;
    }

    // This will knock out the player's speed if they hit anything.
    test_player_tile_collision();

    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);


    if (rawXPosition > SCREEN_EDGE_RIGHT) {
        // We use sprite direction to determine which direction to scroll in, so be sure this is set properly.
        playerDirection = SPRITE_DIRECTION_RIGHT;
        gameState = GAME_STATE_SCREEN_SCROLL;
        playerOverworldPosition+=8;
    }
}

void test_player_tile_collision() {

	if (playerYVelocity != 0) {
		collisionTempY = ((playerYPosition + playerYVelocity) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
		collisionTempX = ((playerXPosition) >> PLAYER_POSITION_SHIFT);
        collisionTempYBottom = ((playerYPosition + playerYVelocity + PLAYER_HEIGHT_EXTENDED) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((playerXPosition + PLAYER_WIDTH_EXTENDED) >> PLAYER_POSITION_SHIFT);
        // TODO: Do we need to do clever clamping with collisionTempY here like we do with X below? Need to see how it behaves when we add vertical scrolling
		if (playerYVelocity < 0) {
            // We're going up - test the top left, and top right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1)) {
                playerYVelocity = 0;
                playerVelocityLockTime = 0;
            }
            playerDirection = SPRITE_DIRECTION_UP;
		} else {
            // Okay, we're going down - test the bottom left and bottom right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                playerYVelocity = 0;
                playerVelocityLockTime = 0;

			}
            playerDirection = SPRITE_DIRECTION_DOWN;
		}
    }

	if (playerXVelocity != 0) {
		collisionTempX = (playerXPosition + playerXVelocity) >> PLAYER_POSITION_SHIFT;
		collisionTempY = ((playerYPosition + playerYVelocity) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempYBottom = ((playerYPosition + playerYVelocity + PLAYER_HEIGHT_EXTENDED) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((playerXPosition + playerXVelocity + PLAYER_WIDTH_EXTENDED) >> PLAYER_POSITION_SHIFT);


        // Depending on how far to the left/right the player is, there's a chance part of our bounding box falls into
        // the next column of tiles on the opposite side of the screen. This if statement prevents those collisions.
        if (collisionTempX > 2 && collisionTempX < 238) {
            if (playerXVelocity < 0) {
                // Okay, we're moving left. Need to test the top-left and bottom-left
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerVelocityLockTime = 0;

                }
                playerDirection = SPRITE_DIRECTION_LEFT;
            } else {
                // Going right - need to test top-right and bottom-right
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerVelocityLockTime = 0;

                }
                playerDirection = SPRITE_DIRECTION_RIGHT;
            }
        }
	}

    playerXPosition += playerXVelocity;
    playerYPosition += playerYVelocity;

}