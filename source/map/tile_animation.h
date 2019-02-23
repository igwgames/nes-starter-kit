#define PRG_BANK_MAP_TILE_ANIMATION 2

// These are the two chr banks that we want to flip between. Look at 
// crt0.asm to see which banks line up to which files.
#define CHR_ANIMATION_BANK_0 1
#define CHR_ANIMATION_BANK_1 3

void animate_map_tiles(void);