---
title: Understanding and Tweaking the build tools
permalink: guide/section_5/build_tools.html
---
# Understanding and Tweaking the build tools

There are a number of tools used to build this game, and they can be tweaked to improve your game.
The key tool to be aware of is `create-nes-game`, which you may have installed,
 but we also have a number of other tools
that are worth mentioning. This chapter will try to briefly cover all of them.

## The root of everything: the `.create-nes-game.config.json` file

The keystone for building your game is the file named `.create-nes-game.config.json` in the root directory 
of the game. 
This file contains instructions for building all of the little pieces of the game, and to put those
things together. It is used by `create-nes-game` to understand what we need.

Most of what you will want to change relates to actions.

See the 
[create-nes-game documentation](https://cppchriscpp.github.io/create-nes-game/#/?id=available-actions)
for details on things you can do. 

We'll take a closer look at the actions we use for this project below.

## Custom build tools

There are a number of small javascript utilities we use to build the rom and some other useful things. 
Each of them has its own section below. They are built using nodejs version 12 (later should also be 
fine.)

The tools have [their own repository](https://cppchriscpp.github.io/nes-starter-kit-tools).

To make life easier, each of these utilities is compiled into a single executable file that is 
automatically downloaded by create-nes-game. This means users don't have to install nodejs unless they want to 
edit these tools. These executables are built using a tool called 
[pkg](https://github.com/zeit/pkg). The exact instructions are in the README for the repo.

If you want to edit these tools, you will need to install nodejs 12.x or later onto your machine.
You will also need to tweak your `.create-nes-game.config.json` to use node to run the tools directly.

Instructions are in the 
[nes-starter-kit-tools repository README](https://cppchriscpp.github.io/nes-starter-kit-tools)

### CHR2IMG

This tool is used to convert the NES-formatted chr files edited in NESST into images that
our tile editor can understand. It prints out a png containing a copy of the tile data in all 4
palettes in whatever palette file is passed in. The program will print out its command line
arguments if you pass in the `--help` parameter. 

All of its code is in the `index.js` file in its folder. It is unfortunately
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

The tool itself writes these extra bits like this, if you want to search for it:

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

All of its code is in the `index.js`. It is unfortunately
not well documented, and right now that is unlikely to change. It works for our purposes, so
for now that is deemed "good enough."
