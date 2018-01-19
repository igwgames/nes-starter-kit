/*
main.c is the entrypoint of your game. Everything starts from here.
This has the main loop for the game, which is then used to call out to other code.
*/

#include "tools/neslib_famitracker/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/menus/title.h"

// These are special "ZEROPAGE" variables. They are faster to access, but there is very limited space for them.
// (255 bytes, many of which are used by the engine)
// Convert your most heavily used variables using the ZEROPAGE_DEF method. 
// (Usage: ZEROPAGE_DEF(type, variableName) ; eg ZEROPAGE_DEF(int, myInt))
// Note: If you are using a variable as a global, you also need to change it in `globals.h`!
ZEROPAGE_DEF(unsigned char, gameState);

void main() {
    gameState = GAME_STATE_SYSTEM_INIT;

    while (1) {

        switch (gameState) {
            case GAME_STATE_SYSTEM_INIT:
                gameState = GAME_STATE_TITLE_DRAW;
                break;

            case GAME_STATE_TITLE_DRAW:
                banked_call(PRG_BANK_TITLE, draw_title_screen);
                break;
            case GAME_STATE_TITLE_INPUT:
                banked_call(PRG_BANK_TITLE, handle_title_input);
                break;
        }
        ppu_wait_frame();
    }
}
