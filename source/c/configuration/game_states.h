// These are various states the game can be in. The loop in main.c determines what to
// do each cycle based on this. 
#define GAME_STATE_SYSTEM_INIT      0
#define GAME_STATE_TITLE_DRAW       10
#define GAME_STATE_TITLE_INPUT      11
#define GAME_STATE_POST_TITLE       20
#define GAME_STATE_RUNNING          50
#define GAME_STATE_SHOWING_TEXT     60
#define GAME_STATE_PAUSED           100
#define GAME_STATE_SCREEN_SCROLL    150
#define GAME_STATE_GAME_OVER        201
#define GAME_STATE_CREDITS          250