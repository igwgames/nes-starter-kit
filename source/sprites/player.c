#include "source/neslib_asm/neslib.h"
#include "source/sprites/player.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/collision.h"
#include "source/sprites/sprite_definitions.h"
#include "source/sprites/map_sprites.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);
ZEROPAGE_DEF(unsigned char, playerVelocityLockTime);
ZEROPAGE_DEF(unsigned char, playerDirection);

// TODO: How much of this could use temp vars? I'd guess most...
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
    // Using a variable, so we can change the velocity based on pressing a button, having a special item,
    // or whatever you like!
    int maxVelocity = PLAYER_MAX_VELOCITY;
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }

    if (controllerState & PAD_RIGHT && playerXVelocity >= 0) {
        // If you're moving right, and you're not at max, speed up.
        if (playerXVelocity < maxVelocity) {
            playerXVelocity += PLAYER_VELOCITY_ACCEL;
        // If you're over max somehow, we'll slow you down a little.
        } else if (playerXVelocity > maxVelocity) {
            playerXVelocity -= PLAYER_VELOCITY_ACCEL;
        }
    } else if (controllerState & PAD_LEFT && playerXVelocity <= 0) {
        // If you're moving left, and you're not at max, speed up.
        if (ABS(playerXVelocity) < maxVelocity) {
            playerXVelocity -= PLAYER_VELOCITY_ACCEL;
        // If you're over max, slow you down a little...
        } else if (ABS(playerXVelocity) > maxVelocity) { 
            playerXVelocity += PLAYER_VELOCITY_ACCEL;
        }
    } else if (playerXVelocity != 0) {
        // Not pressing anything? Let's slow you back down...
        if (playerXVelocity > 0) {
            playerXVelocity -= PLAYER_VELOCITY_ACCEL;
        } else {
            playerXVelocity += PLAYER_VELOCITY_ACCEL;
        }
    }

    if (controllerState & PAD_UP) {
        if (ABS(playerYVelocity) < maxVelocity) {
            playerYVelocity -= PLAYER_VELOCITY_ACCEL;
        } else if (ABS(playerYVelocity) > maxVelocity) {
            playerYVelocity += PLAYER_VELOCITY_ACCEL;
        }
    } else if (controllerState & PAD_DOWN) {
        if (playerYVelocity < maxVelocity) {
            playerYVelocity += PLAYER_VELOCITY_ACCEL;
        } else if (playerYVelocity > maxVelocity) {
            playerYVelocity -= PLAYER_VELOCITY_ACCEL;
        }
    } else { 
        if (playerYVelocity > 0) {
            playerYVelocity -= PLAYER_VELOCITY_ACCEL;
        } else if (playerYVelocity < 0) {
            playerYVelocity += PLAYER_VELOCITY_ACCEL;
        }
    }

    // This will knock out the player's speed if they hit anything.
    test_player_tile_collision();
    handle_player_sprite_collision();

    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);


    if (rawXPosition > SCREEN_EDGE_RIGHT) {
        // We use sprite direction to determine which direction to scroll in, so be sure this is set properly.
        playerDirection = SPRITE_DIRECTION_RIGHT;
        gameState = GAME_STATE_SCREEN_SCROLL;
        playerOverworldPosition+=8;
    } else if (rawXPosition < SCREEN_EDGE_LEFT) {
        playerDirection = SPRITE_DIRECTION_LEFT;
        gameState = GAME_STATE_SCREEN_SCROLL;
        playerOverworldPosition -= 8;
    } else if (rawYPosition > SCREEN_EDGE_BOTTOM) {
        playerDirection = SPRITE_DIRECTION_DOWN;
        gameState = GAME_STATE_SCREEN_SCROLL;
        playerOverworldPosition++;
    } else if (rawYPosition < SCREEN_EDGE_TOP) {
        playerDirection = SPRITE_DIRECTION_UP;
        gameState = GAME_STATE_SCREEN_SCROLL;
        playerOverworldPosition--;
    }
}

void test_player_tile_collision() {

	if (playerYVelocity != 0) {
		collisionTempY = ((playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
		collisionTempX = ((playerXPosition + PLAYER_X_OFFSET_EXTENDED) >> PLAYER_POSITION_SHIFT);
        collisionTempYBottom = ((playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity + PLAYER_HEIGHT_EXTENDED) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((playerXPosition + PLAYER_X_OFFSET_EXTENDED + PLAYER_WIDTH_EXTENDED) >> PLAYER_POSITION_SHIFT);
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
		collisionTempX = (playerXPosition + PLAYER_X_OFFSET_EXTENDED + playerXVelocity) >> PLAYER_POSITION_SHIFT;
		collisionTempY = ((playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempYBottom = ((playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity + PLAYER_HEIGHT_EXTENDED) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((playerXPosition + PLAYER_X_OFFSET_EXTENDED + playerXVelocity + PLAYER_WIDTH_EXTENDED) >> PLAYER_POSITION_SHIFT);


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

#define currentMapSpriteIndex tempChar1
void handle_player_sprite_collision() {
    if (lastPlayerSpriteCollisionId != NO_SPRITE_HIT) {
        currentMapSpriteIndex = lastPlayerSpriteCollisionId<<MAP_SPRITE_DATA_SHIFT;
        // TODO: Different sprite types, mayhaps?
        if (playerHealth < playerMaxHealth) {
            playerHealth++;
            currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;
        }

    }
}