# Saving the Game

Sometimes adventure games are big - really big, and you don't want to complete them all in one sitting.
Some NES games let you save your game, then come back later. It turns out this isn't hard to do at all,
so you can use it in your own game! 

The default options for the cartridge support up to 8kb (8192 bytes) worth of save ram. If you power
the console off, then turn it back on, the values in this ram will stay the same. We of course don't look
directly at RAM. We instead care about variables - for us this means that specially marked variables will
keep the same value even after the console is turned off. Cool, huh? You could also use this RAM as additional
regular RAM if you wanted to - you just have to make sure to clear it on system startup.

We can demonstrate this by adding a counter that shows how many times the game has been beaten. You could use
the same method to save any other data you wanted. 

## How to do it

Normally, when defining a variable, you would do the following to define it: 
```c
unsigned char gamesBeaten;
```

This is really close to what we need to do for this example - we just need a little bit of extra helper
code to make the variable a saved variable. (This is enabled by a macro built into the engine.) Here's what
you need to do to keep the variable saved between resets: 

```c
SRAM_DEF(unsigned char, gamesBeaten);
```

Put this into `source/globals.c`, so we can use it when we need it.

Also be sure to add the following to `source/globals.h` so we can actually use the variable from other C code:

```c
SRAM_EXTERN(unsigned char, gamesBeaten);
```

This is very similar to the delcarations we do for ZEROPAGE variables. That's it, actually - any value
you store into the `gamesBeaten` variable will be saved until the next time it is overwritten. How can we 
prove this though? Let's start setting it! 

When a variable is first defined, it has an unknown value - it could be 59, or 255, or anything!
That's not ideal. There is a section further down in this chapter describing how to detect this and clear
your variables properly, however for now let's just set the value once to put it into a known state.

_Note: Using save states or clearing your save game during this tutorial could put you into an unknown
state. Make sure you read the rest of this chapter to make sure that doesn't happen to players of your game!_

Let's add this to our `main()` function in `source/main.c`, to set the value once to a known value. Add this
to the very top of it: 

```c
void main(void) {
    gamesBeaten = 0;

    fade_out_instant();
    gameState = GAME_STATE_SYSTEM_INIT;

    // And so on...
```

Now, open your game, and get past the title menu once. Then, close it. Your emulator should have just
saved the state of the game in a .sav file. (You may see it next to the ROM in the `rom` folder now.)

**Once that is done, remove the line from `main()` that you just added; we don't need it anymore!**

Now, we need to increase this counter every time you beat the game, and also show it somewhere. (The title screen)
Let's start by updating it when you beat the game. To show the credits, we set `gameState` to `GAME_STATE_CREDITS`, and
there is some code to handle this towards the bottom of `source/main.c` - let's add our variable here. Find the case
statement for `GAME_STATE_CREDITS` and add the first line here: 

```c
    case GAME_STATE_CREDITS:
        gamesBeaten++;

        music_stop();
        sfx_play(SFX_WIN, SFX_CHANNEL_1);

```

With that one line, we will track every time you beat the game. Brilliant! Now we just need to show it on the title
screen. This is drawn by the `draw_title_screen()` method in `source/menus/title.c`. We won't go into extreme detail,
but you can draw it by adding the following lines anywhere before we call `ppu_on_all()`: 

```c
    // draw the number of games the player has beaten
    put_str(NTADR_A(6, 22), "Games Beaten: ");
    vram_put((gamesBeaten / 10) + 0x10);
    vram_put((gamesBeaten % 10) + 0x10);
```

Okay, that's it for the basic example! Start the game up, you should see zero. Beat it once, it should go up to one.
Close the emulator and re-open it; you should still see one! Saved! Read on for some more information on how to use
this correctly.

![Games Beaten](../images/games_beaten_3.png)

## Saving arrays of data

If you're doing anything complex, you likely would want to save arrays of data as well. Luckily, it's just as simple.
If you created any arrays in ZEROPAGE (or looked at the code that exists) you can likely guess what it looks like.
In either case, here's an example: 

```c
SRAM_ARRAY_DEF(unsigned char, saveData, 25);
```

This will result in a saved variable that would otherwise look like `unsigned char saveData[25]`. Be sure to clear it
out using a for loop when you are resetting your game's data. Read on for information on that.

## Resetting your data when someone plays for the first time

As was mentioned earlier in this chapter, when you first create a variable in SRAM, it could have any valid value -
for an unsigned character, it could be any value between `0` and `255`; for an integer, anything from `-32768` to 
`32767`. Your game will likely rely on this data, so we can't have that. 

Two common ways of dealing with that are adding a known "magic" value to your SRAM, and tracking a checksum of the
data. In either case, you save this value with the rest of your data, then check it when the game is started. If the 
this value is not what you expect, you can assume the data is wrong, and clear it out. 

Either one will let you set an initial state for your game, and also give you an easy way for you to clear your saved
game. (This is especially important on real cartridges, since clearing the save data on those takes effort.) 

This author prefers the "magic value" method for its simplicity, but both are presented here for your use.

### The simple way, using a magic value

The simplest way you can solve this problem is to write a known value to SRAM, and test if this value is present
when the game is started up. If this value is not exactly equal to your expected value, then you need to clear
all save data and put it into your expected initial state.

**Pros**: 
- Simple, easy to understand
- Decently reliable 
- Easy to clear by changing the "magic" value

**Cons**: 
- Value has to be manually changed every time you add new variables to save
- Won't help with corruption of RAM if it doesn't change your "magic" value

To implement this, we need to add a few new things. This can be combined with the example above to test it;
don't delete that code yet! First, we need to create that magic value in SRAM. Add the following to 
`source/globals.c`:

```c
SRAM_DEF(unsigned int, magicValue);
```

Next we need to create an `extern` version of this number so we can use it from outside. While we're in the file, we
will also create a constant called `EXPECTED_MAGIC_VALUE` which we will compare against. You can set this to any valid 
integer - the value of `100` below is completely arbitrary. Add this to `source/globals.h`:

```c
#define EXPECTED_MAGIC_VALUE 100

SRAM_EXTERN(unsigned int, magicValue);
```

Now, we need to add a way to check the SRAM to see if it needs to be reset. Let's add a method called
`test_and_reset_sram()` to the top of `source/main.c` to test for our magic value, and reset things if they are
not as we'd like: 

```c
void test_and_reset_sram(void) {
    if (magicValue != EXPECTED_MAGIC_VALUE) {
        // Reset all sram values to their expected state here
        gamesBeaten = 0;

        // If you have any array values, use a for loop to set every value in the array correctly.

        magicValue = EXPECTED_MAGIC_VALUE;
    }
}
```

Finally, we can call this method from our `main()` method to make sure it happens every time we start the console.
You could add this in a few places, but I prefer add it to the `GAME_STATE_SYSTEM_INIT` case in the main loop: 

```c
switch (gameState) {
    case GAME_STATE_SYSTEM_INIT:
    
        test_and_reset_sram();

        // Existing code
        initialize_variables();
        gameState = GAME_STATE_TITLE_DRAW;
        break;
```

If you try to boot the game again, you should find that the games beaten value has been reset, but continues to work
as expected. If you change `EXPECTED_MAGIC_VALUE` this will cause your save data to be cleared when the game is next
started up.

I suggest changing `EXPECTED_MAGIC_VALUE` every time you change the values saved in SRAM, or alternatively with every
major version of your game that you release. Warn people that new versions will reset their game progress, of course.
However, a reset game is likely better than a game with corrupted save data!

### The more foolproof way, using checksums

The "magic value" method is very simple and easy to understand, but there is a bit of manual work involved with
changing that value regularly - you can also mess up if you forget to change the value at all. Plus, there is a
miniscule chance that the save ram will just happen to be set to your magic value, which could cause the game
to start with a corrupted save. (This is very unlikely unless you pick a magic value of `0` though.)

If you'd like a little extra protection, in return for a bit of extra code an maintenance, here's how you can do it.

**Pros**: 
- Less likely to run into corrupted save
- Less likely to make mistakes manually updating `EXPECTED_MAGIC_VALUE` (which we don't use)

**Cons**:
- Higher complexity; easier to mess up (resulting in lost save data)
- Maintenance effort a bit higher - you need to make sure all values are considered in the checksum
- More code involved in checking value

[Wikipedia](https://en.wikipedia.org/wiki/Checksum) can explain checksums a lot better than I can, but for our
purposes, a checksum is a small value that can be used to validate that the data stored in ram for the game is
not corrupted. (And has been written before.) 

There are many complex ways of doing this, but we're going to go for a dead simple approach where we just add all of 
our stored values together into one integer for comparison. This value will likely overflow multiple times in complex
cases, but that is okay! We only care about the resulting value.

First, we need a variable to store this value in - we'll use an unsigned integer, which should give us some confidence.
Put this into `source/globals.c`:

```c
unsigned int temporarySramChecksum; // NOTE: You could also reuse a tempInt value if you would prefer
SRAM_DEF(unsigned int, sramChecksum);
```

And this into `source/globals.h` to access it; 

```c
SRAM_EXTERN(unsigned int, sramChecksum);
```

Now, we need a method to calculate this checksum. For simplicity we can put this into `source/main.c`, however if you
want to maintain this code, I strongly suggest creating a new file for SRAM data. Create the following method: 

```c
void calculateSramChecksum(void) {
    temporarySramChecksum = 0;

    // Add an entry for every single SRAM value you store here: 
    temporarySramChecksum += gamesBeaten;

    // If you have any arrays, be sure to add every single using a for loop, like in 
    // the following commented-out example
    /*

    for (i = 0; i != 10; ++i) {
        temporarySramChecksum += myArray[i];
    }

    */

    // temporarySramChecksum now contains the checksum value we need.
}
```

This method will allow us to calculate the checksum for all of our SRAM data. _Make sure you update this method every
time you add new SRAM data, or you could end up with corrupted save files._

Now, we need to use this method on startup to verify that our save data is correct, and update it if it isn't. We can 
use something similar to what the magic byte method uses. We will put this in `source/main.c` but it could go in the
same separate file as well. Add a new method called `test_and_reset_sram()`: 

```c
void test_and_reset_sram(void) {
    calculateSramChecksum();

    if (temporarySramChecksum != sramChecksum) {
        // Our save ram is corrupted or blank; reset all save ram for a new game.
        gamesBeaten = 0;

        // If you had an array you were using, you would do this to clear it. (Commented out)
        
        /*

        for (i = 0; i != 10; ++i) {
            myArray[i] = 0;
        }

        */

        calculateSramChecksum();
        sramChecksum = temporarySramChecksum;
    }
}
```

We can call this from `main()` on game startup to make sure that the save RAM is in a known good state. (Look at the
example in the "magic byte" section - the code in `main()` will be identical.

There's one last major step we need to take - **every time we update any value in save ram, we must update the 
checksum, or your save data will be marked as corrupted, and reset!!** We thus need to update any place we change
save ram, and update the checksum. We can create a helper method for this in `source/main.c`: 

```c
void update_sram_checksum(void) {
    calculateSramChecksum();
    sramChecksum = temporarySramChecksum;
}
```

We must call this `update_sram_checksum()` method every single time we update a save ram value. In our `gamesBeaten`
example, we would call this right after. Here's the updated section from `source/main.c`: 

```c
case GAME_STATE_CREDITS:
    gamesBeaten++;
    update_sram_checksum();

    music_stop();

```

It is critical that we remember to update this value every time we update save ram. Otherwise though, this example
should now work. You can re-launch the game, and the value of `gamesBeaten` should update as it did before. Now you
are more protected against save data corruption!
