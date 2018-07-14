// Functions to help with displaying text on the screen when in various menus.

// Put a string on the screen at X/Y coordinates given in adr.
// Usage: put_str(NTADR_A(x, y), "What string to write")
void put_str(unsigned int adr, const char *str);

// Clear the screen to all space characters. Rendering must be off first!
void clear_screen();

// Clear the screen with a border around it - useful for menus, etc.
void clear_screen_with_border();

// Constants around some stuff in the text screen
#define ASCII_TILE_TL   0xe0
#define ASCII_TILE_TR   0xe1
#define ASCII_TILE_BL   0xf0
#define ASCII_TILE_BR   0xf1
#define ASCII_TILE_HORZ 0xf6
#define ASCII_TILE_VERT 0xe6