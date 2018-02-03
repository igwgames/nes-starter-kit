#include "tools/neslib_famitracker/neslib.h"
#include "source/sprites/player.h"
#include "source/configuration/system_constants.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);


void update_player_sprite() {
    // FIXME The player position should probably be set somewhere sane, and like.. change here.
    playerXPosition = playerYPosition = (128 << PLAYER_POSITION_SHIFT);
    
    oam_spr((playerXPosition >> PLAYER_POSITION_SHIFT), (playerYPosition >> PLAYER_POSITION_SHIFT), PLAYER_SPRITE_TILE_ID, 0x00, PLAYER_SPRITE_INDEX);
    oam_spr((playerXPosition >> PLAYER_POSITION_SHIFT) + NES_SPRITE_WIDTH, (playerYPosition >> PLAYER_POSITION_SHIFT), PLAYER_SPRITE_TILE_ID + 1, 0x00, PLAYER_SPRITE_INDEX+4);
    oam_spr((playerXPosition >> PLAYER_POSITION_SHIFT), (playerYPosition >> PLAYER_POSITION_SHIFT) + NES_SPRITE_HEIGHT, PLAYER_SPRITE_TILE_ID + 16, 0x00, PLAYER_SPRITE_INDEX+8);
    oam_spr((playerXPosition >> PLAYER_POSITION_SHIFT) + NES_SPRITE_WIDTH, (playerYPosition >> PLAYER_POSITION_SHIFT) + NES_SPRITE_HEIGHT, PLAYER_SPRITE_TILE_ID + 17, 0x00, PLAYER_SPRITE_INDEX+12);



}