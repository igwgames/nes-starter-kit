// Sets up game state objects to draw text starting during the next frame.
// String is the full text of the message, null terminated. Text will be broken up
// into 30 character lines, and 3 line pages. For example, 
// const unsigned char* myText = "The quick brown fox jumps over" 
//                               "the lazy dog. That dog is     "
//                               "exceedingly lazy, so it does  "
//                               "not move.";
#define PRG_BANK_GAME_TEXT 1

// The arrow text to show when we have text to continue showing...
#define SPRITE_TILE_ID_TEXT_ARROW 0xc2

// The oam id for the tile to be used.
#define TEXT_ARROW_OAM_ID 0x04

// Rather than have a full ASCII table, we skip the first 32 characters so we have a little room for extra 
// data later in the bank. We have to subtract this constantly though, so we made a constant.
#define TEXT_ASCII_SKIPPED_CHARACTERS 0x20

void trigger_game_text(const unsigned char* string);

// Draw text to the screen and handle input for it - this can span multiple screens depending on text length.
void draw_game_text(void);