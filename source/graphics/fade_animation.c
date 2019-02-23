#include "source/graphics/fade_animation.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"

// This file purposely left in the kernel so it can easily be called from anywhere without a bank swap.


// Internal functions that the other ones call with a set number of frames.
void _fade_out(unsigned char speed) {
    for (i = 4; i != 255; --i) {
        delay(speed);
        pal_bright(i);
    }

}

void _fade_in(unsigned char speed) {
    for (i = 0; i != 5; ++i) {
        delay(speed);
        pal_bright(i);
    }

}

void fade_in(void) {
    _fade_in(2);
}

void fade_out(void) {
    _fade_out(2);
}

void fade_out_instant(void) {
    pal_bright(0);
}

void fade_in_slow(void) {
    _fade_in(5);
}

void fade_out_slow(void) {
    _fade_out(5);
}

void fade_in_fast(void) {
    _fade_in(1);
}

void fade_out_fast(void) {
    _fade_out(1);
}