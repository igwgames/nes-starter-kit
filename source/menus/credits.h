
// Draws and handles a very simple pause screen.
#define PRG_BANK_CREDITS_MENU 2

// Draw a screen that says "you won!"
void draw_win_screen();

// Draw a screen that credits the people involved in the game.
void draw_credits_screen();

// Wait for the start button to be pressed.
// TODO: Should this be somewhere else and shared? This has to be a common need...
void handle_credits_input();