#include "temp/level_overworld.h"

// NOTE: For simplicity's sake, we are not putting this into a bank. If you're hurting for space in the kernel, you
// can put this into the map management bank, or another one with a lookup.

// This array contains one entry for each map tile in our map. The one entry is the id of the map tile to teleport 
// to, if a player walks through a teleport door. Ids go from left to right, start at   0, and there are 8 tiles per row.
const unsigned char overworld_warp_locations[] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     17,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0
};