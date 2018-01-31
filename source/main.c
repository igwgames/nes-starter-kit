/*
main.c is the entrypoint of your game. Everything starts from here.
This has the main loop for the game, which is then used to call out to other code.
*/

#include "tools/neslib_famitracker/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/menus/error.h"
#include "source/map/load_map.h"
#include "source/map/map.h"
#include "source/graphics/hud.h"


void initialize_variables() {
    playerOverworldPosition = 1;
    playerHealth = 5;
    playerMaxHealth = 8;
}

void main() {
    gameState = GAME_STATE_SYSTEM_INIT;

    while (1) {

        switch (gameState) {
            case GAME_STATE_SYSTEM_INIT:
                initialize_variables();
                gameState = GAME_STATE_TITLE_DRAW;
                break;

            case GAME_STATE_TITLE_DRAW:
                banked_call(PRG_BANK_TITLE, draw_title_screen);
                break;
            case GAME_STATE_TITLE_INPUT:
                banked_call(PRG_BANK_TITLE, handle_title_input);
                break;
            case GAME_STATE_POST_TITLE:
                load_map();
                ppu_off();
                banked_call(PRG_BANK_MAP_LOGIC, draw_current_map);
                ppu_on_all();
            case GAME_STATE_RUNNING:
                // TODO: Only do this on update
                banked_call(PRG_BANK_HUD, update_hud);
                break;

            default: 
                crash_error(ERR_UNKNOWN_GAME_STATE, ERR_UNKNOWN_GAME_STATE_EXPLANATION, "gameState value", gameState);
                
        }
        ppu_wait_frame();
    }
}
