#include "source/c/globals.h"
#include "source/c/configuration/game_states.h"
#include "source/c/neslib.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/menus/input_helpers.h"

CODE_BANK(PRG_BANK_MENU_INPUT_HELPERS);
void wait_for_start(void) {
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }

        ppu_wait_nmi();

    }
}