#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/map_sprites.h"
#include "source/sprites/sprite_definitions.h"
#include "source/map/map.h"
#include "source/library/bank_helpers.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_MAP_SPRITES);

#define currentMapSpriteIndex tempChar1
#define currentSpriteSize tempChar2
#define currentSpriteTileId tempChar3
#define sprX tempInt1
#define sprY tempInt2

void update_map_sprites() {
    // TODO: Implement flicker somehow.
    for (i = 0; i != MAP_MAX_SPRITES; ++i) {
        currentMapSpriteIndex = i << MAP_SPRITE_DATA_SHIFT;
        sprX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
        sprY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
        currentSpriteSize = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_SIZE_MASK; 
        currentSpriteTileId = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TILE_ID];

        if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TYPE] == SPRITE_TYPE_OFFSCREEN) {
            // Hide it and move on.
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 4);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 8);
            oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, 0, 0, currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 12);
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

        if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + (NES_SPRITE_WIDTH/2),
                (sprY >> SPRITE_POSITION_SHIFT) + HUD_PIXEL_HEIGHT + (NES_SPRITE_HEIGHT/2),
                currentSpriteTileId,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX
            );
        } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT),
                (sprY >> SPRITE_POSITION_SHIFT) + HUD_PIXEL_HEIGHT,
                currentSpriteTileId,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + NES_SPRITE_WIDTH,
                (sprY >> SPRITE_POSITION_SHIFT) + HUD_PIXEL_HEIGHT,
                currentSpriteTileId + 1,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 4
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT),
                (sprY >> SPRITE_POSITION_SHIFT) + HUD_PIXEL_HEIGHT + NES_SPRITE_HEIGHT,
                currentSpriteTileId + 16,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 8
            );
            oam_spr(
                (sprX >> SPRITE_POSITION_SHIFT) + NES_SPRITE_WIDTH,
                (sprY >> SPRITE_POSITION_SHIFT) + HUD_PIXEL_HEIGHT + NES_SPRITE_HEIGHT,
                currentSpriteTileId + 17,
                (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_SIZE_PALETTE] & SPRITE_PALETTE_MASK) >> 6,
                currentMapSpriteIndex + FIRST_ENEMY_SPRITE_OAM_INDEX + 12
            );


        }

        
    }
}