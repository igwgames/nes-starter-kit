// Functions to help with displaying text on the screen when in various menus.

// Put a string on the screen at X/Y coordinates given in adr.
// Usage: put_str(NTADR_A(x, y), "What string to write")
void put_str(unsigned int adr, const char *str);

// Clear the screen to all space characters. Rendering must be off first!
void clear_screen();