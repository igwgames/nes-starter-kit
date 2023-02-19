// Runs a simple fade animation; can be used for switching maps, getting in and out of menus, etc.
// Note that control is completely ceded during this step - your logic will not continue until the fade is complete.

// Does a fade animation with a somewhat normal timing.
void fade_in(void);
void fade_out(void);

// Does a slow fade animation.
void fade_in_slow(void);
void fade_out_slow(void);

// Does the fastest fade animation possible, with one frame per color.
void fade_in_fast(void);
void fade_out_fast(void);

// This can be used to instantly turn the screen black - such as when you're starting the game.
// Not really a true "fade", but a nice shortcut method.
void fade_out_instant(void);