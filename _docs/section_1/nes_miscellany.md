---
title: 
permalink: guide/section_1/nes_miscellany.html
---
# Things to know about the NES and C

Okay, so you've (hopefully) programmed in C, or some other C-like language before. 
(If that statement isn't true, don't be too scared! There is a really good C tutorial
linked from the main readme file; go try that out and come back once you feel more
comfortable.) There are a few details about the console that will help you along,
as well as a few C language features you won't see often elsewhere. 

This may not all make sense right away; that's fine! Just be sure to come back here
when you encounter some of these things, and read up as needed.

## On bytes and Hexidecimal numbers

On the NES (and most computers), everything is stored in one or more things called
bytes. A byte is a number between 0 and 255, and can represent anything from a health
value, to a single letter in a name. A `char` (or `unsigned char`) takes up this
much space. Larger things (like `int`s) are made up of multiple bytes.

The numbers between 0 and 255 are typically represented by numbers between 0 and
9, put together to form bigger numbers. You count them like this:
```
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 
```

Hexidecimal is another way to represent those numbers, that is closer to how a 
computer uses them.  A number of our tools use this format, and we also use 
it in the code quite often as a result. In hexidecimal, rather than a digit being 
from 0-9, digits go from 0-f. Each digit can be one of sixteen values, and represents 
half a byte. In C, hexidecimal numbers are prefixed with `0x`. (For example, 
`0x10` in hexidecimal is `16` in regular base-10 digits.)

Counting the same numbers (0-20, or 0x00 to 0x14) in hexidecimal looks like this: 

```
0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f 0x10
0x11 0x12 0x13 0x14
```

You can also represent larger numbers in hexidecimal. For example, the integer value
of 256 can be represented as `0x100` and 1024 can be repesented as `0x400`.

## There is *very* little RAM available

The NES has 2kb of ram available to it - period. That is not a typo; you can store
at most 2048 unique one byte values in ram at once. (Technically there are ways to 
expand this with special NES cartridges, but this is non-trivial.)

This results in us using some amount of trickery for storage - such as packing multiple
small values into a single byte, and using the `unsigned char` type everywhere. (It takes
up 1 byte. The `int` type takes up 2.)

As a result, you will also need to do some things that otherwise might be bad C 
practice. Here are some examples: 
- Use global variables wherever possible
- Reuse the same variable for multiple things (Use `#define` to make things more readable)
- Avoid using function parameters if a global variable could suffice
- Prefer separate arrays of bytes instead of arrays of structs
- Always use the `const` keyword when defining constant data
- Prefer using `++i` over `i++` - the resulting code is smaller and faster
- Use bit shifting (`<<` and `>>`) in place of multiplication and division whenever possible

## The zero page is a small amount of "special" ram

The NES has 256 bytes that can be accessed a little faster than the rest. This is called
the "zero page" - you will see us define a number of variables with `ZEROPAGE_VAR` - this
will create that variable in this space. This should be used for your most frequently-
accessed variables. The engine uses a lot of this space already, so choose carefully.

## There is limited program space too

The NES has a very limited amount of space for your code - 32 kilobytes of code and data
is visible at a time. Games can be bigger than this (the default rom in this engine 
reserves 128 kilobytes of space) but we have to do some tricks to make this work. 

We do this by separating the code into multiple banks - these are 16 kilobyte sections
of code. The NES can have two of these banks loaded at once. The first 16 kilobyte bank
is always loaded (It is sometimes called the `kernel`, or the fixed bank) - we put all 
of our most heavily used code in this. 

There is also a second slot for a bank- this one can have any one of the other 7 
banks loaded into it. We put most of our code (and other data, such as maps) into one 
of the other 7 banks, then call the function in a special way that knows how to access 
the correct bank.

To set which bank to put code into, we use the `CODE_BANK()` macro, with a number
placed into it. If this is used at the top of a code file, all code winds up in that
bank. To call code in a bank, we use the `banked_call()` method. There are a lot of
examples of this in `source/main.c`.

## Project layout

The project layout is hopefully somewhat simple, but I'll reproduce it with a
little extra detail here, in case it helps

```
- nes-starter-kit
|- .vscode          Some project files and settings for Visual Studio Code users
|- graphics         Graphical assets for the game, in editable with nes screen tool (nesst)
|--- tiles.chr      Game tile assets, used on the map
|--- sprites.chr    Game sprite assets, used on the map
|--- ascii.chr      ASCII characters and such, used for the title screen and menus
|--- generated      Folder for images generated by nes-starter-kit. Used in Tiled mostly.
|--- palettes       Folder containing color palettes for the game
|- guide            This documentation ;)
|- levels           Game levels, editable in Tiled.
|- rom              The game in .nes format, passed to emulators
|- sound            
|--- sfx            Sound effects, in Famitracker/famitone2 format. All in one file
|--- music          Music, in Famitracker format. The `.bin` file in here is loaded by
|                   nes-starter-kit. It can be exported from Famitracker.
|- source           The C and assembly source for the game
|--- configuration  Constants and other things that don't have a better home
|--- graphics       Code around displaying and modifying graphics
|--- library        Tools built into nes-starter-kit to make life easier
|--- map            Code and data used to display levels 
|--- menus          Menu code, and text-based screens (such as the credits)
|--- neslib_asm     The code behind neslib, in assembly language. You can probably skip this at first
|--- sprites        Code around showing/colliding with sprites (including the player's sprite)
|--- globals.c/.h   Global variables available for use anywhere
|--- main.c         Starting point for the game's code, and the main game logic loop.
|- temp             Intermediate assembly/object files go here. You can ignore this.
|- tools            Lots of development tools/etc. 
|- makefile         Instructions telling the make command how to build the game
```

## Feeling overwhelmed?

Don't stress out if a lot of this doesn't make sense yet - really, it's ok. Just
start exploring the other chapters and start tweaking the game, then come back
here when you hit something strange. Try to keep the high-level details in mind,
so you know when to look back. 

The next chapter is a frequently asked questions guide. Skimming the question
titles is probably a good idea, since there is a decent chance you will have
one of those questions at some point soon. 

Beyond that, skip on to section 2, and start building your game!