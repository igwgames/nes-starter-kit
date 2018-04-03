#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/map_sprites.h"
#include "source/sprites/player.h"
#include "source/sprites/sprite_definitions.h"
#include "source/map/map.h"
#include "source/library/bank_helpers.h"
#include "source/menus/error.h"
#include "source/sprites/collision.h"

CODE_BANK(PRG_BANK_MAP_SPRITES);

#define currentMapSpriteIndex tempChar1
#define currentSpriteSize tempChar2
#define currentSpriteTileId tempChar3
#define oamMapSpriteIndex tempChar4
#define currentSpriteType tempChar5
#define currentSpriteData tempChar6
#define sprX tempInt1
#define sprY tempInt2
// NOTE: width = height for our examples, so both are set to the same value.
// If you change this, be sure to assign it in the for loop below as well.
#define currentSpriteFullWidth tempInt3
#define currentSpriteFullHeight tempInt3


ZEROPAGE_DEF(unsigned char, lastPlayerSpriteCollisionId);

void update_map_sprites() {
    lastPlayerSpriteCollisionId = NO_SPRITE_HIT;
    for (i = 0; i != MAP_MAX_SPRITES; ++i) {
        currentMapSpriteIndex = i << MAP_SPRITE_DATA_SHIFT;
        
        // This switches what position we write the sprite to regularly, so we can maintain a flicker effect instead
        // of having the sprite just randomly disappear. We use 0x02 so it flips every other frame, so flickering is less
        // likely to fail if we lose a frame. (If you have enough sprites to flicker, you may also see slowdown.)
        if (frameCount & 0x02) {
            oamMapSpriteIndex = (MAP_MAX_SPRITES-i-1) << MAP_SPRITE_OAM_SHIFT;
        } else {
            oamMapSpriteIndex = i << MAP_SPRITE_OAM_SHIFT;
        }
        sprX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
        sprY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
        currentSpriteSize = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_SIZE_MASK; 
        currentSpriteTileId = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TILE_ID];

        if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TYPE] == SPRITE_TYPE_OFFSCREEN) {
            // Hide it and move on.
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 4);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 8);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 12);
            continue;
        }

        switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE]) {
            case SPRITE_ANIMATION_SWAP:

                if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
                    currentSpriteTileId += ((frameCount & 0x10) >> 4);
                } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
                    currentSpriteTileId += ((frameCount & 0x10) >> 3);
                }

                break;
            case SPRITE_ANIMATION_SWAP_FAST:
                if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
                    currentSpriteTileId += ((frameCount & 0x08) >> 3);
                } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
                    currentSpriteTileId += ((frameCount & 0x08) >> 2);
                }
                break;
            case SPRITE_ANIMATION_NONE:
            default: 
                break;

        }
        switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_MOVEMENT_TYPE]) {
            case SPRITE_MOVEMENT_LEFT_RIGHT:
                // Get the speed to travel at
                currentSpriteData = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SLIDE_SPEED];

                // If it's positive, add to X to get the right of the sprite
                if ((signed char) currentSpriteData > 0) {
                    sprX += currentSpriteFullWidth;
                }
                // Add speed in
                sprX += (signed char)currentSpriteData;
                if (test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY)], 0) || test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY + currentSpriteFullHeight)], 0)) {
                    // Never mind... leave X position alone for now
                    sprX -= (signed char)currentSpriteData;
                    // Roll back our change to pick right of the sprite
                    if ((signed char) currentSpriteData > 0) {
                        sprX -= currentSpriteFullWidth;
                    }

                    // And... flip the direction!
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SLIDE_SPEED] = 0 - (signed char)currentSpriteData;
                } else {
                    // No collision! Roll back our change to pick right of the sprite
                    if ((signed char) currentSpriteData > 0) {
                        sprX -= currentSpriteFullWidth;
                    }


                    // And move the sprite over!
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X] = (sprX & 0xff);
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X+1] = (sprX >> 8);
                }

                break;
            case SPRITE_MOVEMENT_UP_DOWN:
                // Get the speed to travel at
                currentSpriteData = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SLIDE_SPEED];

                // If it's positive, add to X to get the right of the sprite
                if ((signed char) currentSpriteData > 0) {
                    sprY += currentSpriteFullWidth;
                }
                // Add speed in
                sprY += (signed char)currentSpriteData;
                if (test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY)], 0) || test_collision(currentMap[SPRITE_MAP_POSITION(sprX + currentSpriteFullWidth, sprY)], 0)) {
                    // Never mind... leave X position alone for now
                    sprY -= (signed char)currentSpriteData;
                    // Roll back our change to pick right of the sprite
                    if ((signed char) currentSpriteData > 0) {
                        sprY -= currentSpriteFullWidth;
                    }

                    // And... flip the direction!
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SLIDE_SPEED] = 0 - (signed char)currentSpriteData;
                } else {
                    // No collision! Roll back our change to pick right of the sprite
                    if ((signed char) currentSpriteData > 0) {
                        sprY -= currentSpriteFullWidth;
                    }


                    // And move the sprite over!
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y] = (sprY & 0xff);
                    currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y+1] = (sprY >> 8);
                }

                break;

            case SPRITE_MOVEMENT_NONE:
            default:
                break;
        }

        if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + (NES_SPRITE_WIDTH/2),
                (sprY >> SPRITE_POSITION_SHIFT) + (NES_SPRITE_HEIGHT/2),
                currentSpriteTileId,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX
            );
        } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT),
                (sprY >> SPRITE_POSITION_SHIFT),
                currentSpriteTileId,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + NES_SPRITE_WIDTH,
                (sprY >> SPRITE_POSITION_SHIFT),
                currentSpriteTileId + 1,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 4
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT),
                (sprY >> SPRITE_POSITION_SHIFT) + NES_SPRITE_HEIGHT,
                currentSpriteTileId + 16,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 8
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + NES_SPRITE_WIDTH,
                (sprY >> SPRITE_POSITION_SHIFT) + NES_SPRITE_HEIGHT,
                currentSpriteTileId + 17,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                oamMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 12
            );


        }

        // While we have all the data above, let's see if we hit anything.
        
        // Only test collision for sprite types that collide.
        currentSpriteType = currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE];
        if (currentSpriteType != SPRITE_TYPE_NOTHING && currentSpriteType != SPRITE_TYPE_OFFSCREEN) {

            // TODO: Make sprites a little bit smaller so they're harder to hit? (Maybe only if they're enemies/damaging)
            // NOTE: we're only setting currentSpriteFullWidth here because our code assumes everything is a square. If you 
            // change that, be sure to change currentSpriteFullHeight here, and give it a new variable above.
            if ((currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_SIZE_MASK) == SPRITE_SIZE_8PX_8PX) {
                currentSpriteFullWidth = NES_SPRITE_WIDTH << PLAYER_POSITION_SHIFT;
            } else {
                currentSpriteFullWidth = NES_SPRITE_WIDTH << (PLAYER_POSITION_SHIFT+1);
            }
            // Collision test... see here for a clear explanation: https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
            // rect1=player position, rect2=sprite position
            if (
                playerXPosition < sprX + currentSpriteFullWidth &&
                playerXPosition + PLAYER_WIDTH_EXTENDED > sprX &&
                playerYPosition < sprY + currentSpriteFullHeight &&
                playerYPosition + PLAYER_HEIGHT_EXTENDED > sprY
            ) {
                lastPlayerSpriteCollisionId = i;
            }
        }

        
    }
}