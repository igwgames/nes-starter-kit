# Understanding and Tweaking the build tools

There are a number of tools used to build this game, and they can be tweaked in many ways to improve your game.
The key tool to be aware of is the `makefile` in the root direcotory, but we also have a number of other tools
that are worth mentioning. This chapter will try to briefly cover all of them.

## The root of everything: the makefile

The keystone for building your game is the file named `makefile` in the root directory of the game. 
This file contains instructions for building all of the little pieces of the game, and to put those
things together. It also knows how to do this without rebuilding everything. (Which makes builds much faster!)

This chapter is not going to be an introduction to makefiles - if you need that, there is a great
one available [here](https://www3.nd.edu/~zxu2/acms60212-40212/Makefile.pdf). 

The file starts with a small set of user-editable settings, the most notable being the name of the ROM.
These can be changed without risk. They are pretty self-explanatory. 

Next up is a bunch of macros for running our various programs. They generally look like this: 

```makefile
MAIN_COMPILER=./tools/cc65/bin/cc65
```

We reference thse guys later on. Of special note are the three javascript tools in here: `CHR2IMG`,
`TMX2C` and `SPRITE_DEF2IMG`. These are referenced later in the guide, and you can change how they
run to skip the executable versions here. 

We also define a bunch of files that we will user later to build the game. We build them up by reading
the directory structure, which protects us from having to add every file we create to this makefile
manually. (Not a fun task.) That looks more like this:

```makefile
SOURCE_LEVELS_TMX=$(strip $(call rwildcard, levels/, *.tmx))
```
`rwildcard` is a macro that finds all files in a directory matching a filename, in a recursive way. This
allows us to match things nested into directories automatically too.

Further down the file, we see a couple shortcut methods that are used to build the rom itself. They look
like this: 

```makefile
build: rom/$(ROM_NAME).nes graphics/generated/tiles.png graphics/generated/sprites.png

build-tiles: graphics/generated/tiles.png
build-sprites: graphics/generated/sprites.png
```

That first one is what runs every time we build the rom. This basically tells our makefile to build
everything it knows about. This is our rom, as well as the two images used by Tiled. It's meant
as a catch-all.

Below this, there are rules for building the various parts of our game. You basically put the file
you want to build on the left, then all files that go into it on the right. That way, if a file on
the right changes, the files on the left are rebuilt. Of note, `crt0.o` has a special rule - this
is the entry point to our game in C. Basically any assembly code you write will end up here. (If it
does not go into its own bank, anyway.)

There are special rules for levels, the tile/sprite images Tiled uses, and sound effect conversion.
Generally intermediate files are stored in the `temp` directory to not mix them with real code files.
Code is compiled using cc65 - it is first converted to assembly language, (.s files) then this assembly is
converted to raw object files.

The `clean` command is used to clean up all temporary and output files, so that we can rebuild the game
from scratch. If you add temporary files in a new location, you should add things to the clean method to 
make sure those are cleaned up.

## Other custom build tools

There are a number of small javascript utilities we use to build the rom and some other useful things. 
Each of them has its own section below. They are built using nodejs version 8 (later should also be 
fine.) 

To make life easier, each of these utilities is compiled into a single executable file that is 
distributed in the tool zip file. This means users don't have to install nodejs unless they want to 
edit these tools. These executables are built using a tool called 
[pkg](https://github.com/zeit/pkg). The exact instructions are shared in the `README.md` files in
each tool's directory. 

If you want to edit these tools, you will need to install nodejs 8.x or later onto your machine.
You will also need to tweak your makefile to use node to run the tools directly. Look for some
lines like this to uncomment: 

```makefile
# Javascript versions of built-in tools: (Uncomment these if you're working on the tools)
# CHR2IMG=node tools/chr2img/src/index.js
# TMX2C=node tools/tmx2c/src/index.js
# SPRITE_DEF2IMG=node tools/sprite_def2img/src/index.js
```

You can just use them in place with javascript if you have it installed; there's no reason not
to. Distributing the executables was only meant to make the tools easier to install and use.

### CHR2IMG

This tool is used to convert the NES-formatted chr files edited in NESST into images that
our tile editor can understand. It prints out a png containing a copy of the tile data in all 4
palettes in whatever palette file is passed in. The program will print out its command line
arguments if you pass in the `--help` parameter. 

All of its code is in the `index.js` file in its `tools/chr2img` folder. It is unfortunately
not well documented, and right now that is unlikely to change. It works for our purposes, so
for now that is deemed "good enough."

### TMX2C

This tool is responsible for converting the map files created by Tiled into a format that our game
can understand directly. It takes in a map file and some other details, and prints out a C file
that has it in a format our game understands. 

Each map tile takes up 256 bytes, so incrementing the MSB by one will move us right one map tile.
The output file actually tries to remain readable, and also has a little room for more data. Here
is an example: 

```c
    //Room (0:0)
    
    // Room data
    
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    100, 100, 3, 3, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 3, 3,
    100, 100, 3, 3, 153, 165, 129, 129, 129, 129, 129, 129, 165, 153, 3, 3,
    100, 100, 3, 3, 153, 129, 129, 129, 129, 129, 129, 129, 129, 153, 3, 3,
    100, 100, 3, 3, 153, 129, 129, 129, 129, 129, 129, 129, 129, 153, 3, 3,
    100, 100, 3, 3, 153, 129, 129, 129, 129, 129, 129, 129, 129, 153, 3, 3,
    100, 100, 3, 3, 153, 153, 153, 153, 129, 129, 153, 153, 153, 153, 3, 3,
    100, 100, 3, 3, 3, 3, 3, 3, 129, 129, 3, 3, 3, 3, 3, 3,
    100, 100, 3, 3, 3, 3, 3, 3, 129, 129, 3, 3, 3, 3, 3, 3,
    100, 100, 3, 3, 3, 3, 3, 3, 129, 129, 3, 3, 3, 3, 3, 3,
    
    // Sprite Data
    86, 0, 91, 0, 102, 0, 107, 0, 136, 6, 137, 6, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    // add a little padding - we want each map screen to take up exactly 256 bytes to make math easier.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
```

The top chunk is the data for the actual map. It uses 16x16 tiles, numbered 0-63. We use the top
two bits to determine the palette number, so effectively 0-63 use palette 1, 64-127 use palette 2,
and so on.

The next section is the information for sprites on the screen. Each sprite takes up two bytes - 
the first is the 16x16 tile index to start the sprite on. The second is the index of the sprite in
`sprites/sprite_definitions.c`. A value of `255` is used for sprites that are offscreen. The final
32 bytes are currently unused, and could be filled out with whatever data you want. 

The tool itself writes these extra bits like this, if you want to search `tools/tmx2c/src/index.js` for
it:

```javascript
// Lastly, we want things to line up perfectly so we have 256 per map tile (makes math easier)
// So, add some padding. (Note: If you wanna add your own data, this is the spot!)
mapData += '// add a little padding - we want each map screen to take up exactly 256 bytes to make math easier.\n';
mapData += '0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n';
mapData += '0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n\n';
```

You can then use your engine to read this data - it will be automatically available within 
`currentMap` at indexes `224` through `255` after you call `load_map()`.

### SPRITE_DEF2IMG

This tool is quite tied to our game's code. It takes in the information in `sprites/sprite_definitions.c`
alongside a chr file of your sprites, and converts this into a `.png` image that Tiled knows how to
read.  The program will print out its command line arguments if you pass in the `--help` parameter. A
large amount of the code is shared with `chr2img`.

All of its code is in the `index.js` file in its `tools/sprite_def2png` folder. It is unfortunately
not well documented, and right now that is unlikely to change. It works for our purposes, so
for now that is deemed "good enough."
