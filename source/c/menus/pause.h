// Draws and handles a very simple pause screen.
#define PRG_BANK_PAUSE_MENU 2

// The tile to draw for the selected menu item.
#define MENU_ARROW_TILE 0xe2
// The tile to show for empty space (ascii tiles are 32 bytes off, so we have to remove that here)
#define MENU_BLANK_TILE ' ' - 0x20

void draw_pause_screen(void);

void handle_pause_input(void);