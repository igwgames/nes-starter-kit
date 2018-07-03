# Saving the Game

Sometimes adventure games are big - really big, and you don't want to complete them all in one sitting.
Some NES games let you save your game, then come back later. It turns out this isn't hard to do at all,
so you can use it in your own game! 

The default options for the cartridge support up to 2kb (2048 bytes) worth of save ram. If you power
the console off, then turn it back on, the values in this ram will stay the same. We of course don't look
directly at RAM. We instead care about variables - for us this means that specially marked variables will
keep the same value even after the console is turned off. Cool, huh?

_Note: Technically, the cartridge has 8k (8096) bytes of save ram available, howevver that requires banking,
which we will not go into here._

We can demonstrate this by adding a timer that saves the fastest time to beat the game. You could use
this same method to save as much data as you need within cartridge limits.

## How to do it

Normally, when defining a variable, you would do the following to define it: 
```c
unsigned int fastestTime;
```

This is really close to what we need to do for this example - we just need a little bit of extra helper
code to make the variable a saved variable. (This is enabled by a macro built into the engine.) Here's what
you need to do to keep the variable saved between resets: 

```c
SRAM_DEF(unsigned int, fastestTime);
```

Put this into `source/globals.c`, so we can use it when we need it. We'll also need a regular non-saved variable
to track how long the current game has gone, so add this as well: 
```c
unsigned int currentGameTime;
```

Also be sure to add the following
to `source/globals.h` so we can actually use these two variables: 

```c
// TODO: Should we have a SRAM_EXTERN function? It wouldn't technically do anything, but might be clearer...
extern unsigned int fastestTime;

extern unsigned int currentGameTime;
```

This is very similar to the delcarations we do for ZEROPAGE variables. That's it, actually - any value
you store into the `fastestTime` variable will be saved until the next time it is overwritten. How can we 
prove this though? Let's start setting it! 

When a variable is first defined, it has an unknown value - it could be 59, or 9999, or anything!
That's not ideal. There is a section further down in this chapter describing how to detect this and clear
your variables properly, however for now let's just set the value once to put it into a known state.

_Note: Using save states or clearing your save game during this tutorial could put you into an unknown
state. Make sure you read the rest of this chapter to make sure that doesn't happen to players of your game!_

Let's add this to our `main()` function in `source/main.c`, to set the value once to a known value. Add this
to the top of it: 

```c
void main() {
    fastestTime = 65535; // This is the maximum value for an unsigned integer

    fade_out_instant();
    gameState = GAME_STATE_SYSTEM_INIT;

    // And so on...
```

Now, open your game, and get past the title menu once. Then, close it. Your emulator should have just
saved the state of the game in a .sav file. (You may see it next to the ROM in the `rom` folder now.)
**Once that is done, remove the line from `main()` that you just added; we don't need it anymore!**

Now, we need to add a little code to actually time gameplay, and to start saving to this value.

TODO: Would it be easier to just show the number of times the game has been beaten? Probably! Maybe
we do that instead...

## Saving arrays of data

TODO!!
```c
SRAM_DEF(unsigned char, theThing, 25);
// unsigned char theThing[25];
```

## Resetting your data when someone plays the game for the first time

### The lazy way, using a magic byte

### The smart way, using checksums

## Note on using save files with different game versions