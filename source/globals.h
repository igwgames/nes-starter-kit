// This file contains global variables that can be accessed from anywhere. 
// You will probably have a lot of variables in here, as the NES has very limited RAM.
// It is often a good idea to reuse a variable for multiple small things, rather than have a bunch. 
// (Especially if you put that variable in zeropage)
#include "source/library/bank_helpers.h"

// If you create variables as ZEROPAGE using the ZEROPAGE_DEF method, be sure to also set them as
// externs using the ZEROPAGE_EXTERN method below. This replaces the regular extern format.
ZEROPAGE_EXTERN(unsigned char, gameState);

ZEROPAGE_EXTERN(unsigned char, i);
ZEROPAGE_EXTERN(unsigned char, j);
ZEROPAGE_EXTERN(unsigned char, playerHealth);
ZEROPAGE_EXTERN(unsigned char, playerMaxHealth);

ZEROPAGE_ARRAY_EXTERN(unsigned char, screenBuffer, 0x55);

ZEROPAGE_EXTERN(unsigned char, controllerState);
ZEROPAGE_EXTERN(unsigned char, lastControllerState);

// Bit of a special case - this is exported from assembly in crt0.asm - it's the number of frames since the system started.
// Note: DO NOT try to alter this with your code; you will almost certainly cause unusual behavior.
ZEROPAGE_EXTERN(unsigned int, frameCount);

// TODO: There might be a nicer home for these
// Get the absolute value of a number, the lazy person's way.
#define ABS(x) (x > 0 ? x : 0-x)