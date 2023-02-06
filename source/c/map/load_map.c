#include "source/c/configuration/system_constants.h"
#include "source/c/neslib.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/map/map.h"
#include "source/c/globals.h"

// Loads the map at the player's current position into the ram variable given. 
// Kept in a separate file, as this must remain in the primary bank so it can
// read data from another prg bank.
void load_map(void) {
    
    // Need to switch to the bank that stores this map data.
    bank_push(currentWorldId);
    memcpy(currentMap, overworld + (playerOverworldPosition << 8), 256);
    bank_pop();

}