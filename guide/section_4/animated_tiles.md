# Animating Tiles

![animated tiles](../images/animated_tiles.gif)

Animating tiles is a simple way to make your game stand out. A little bit of movement in a level can make
it feel much more natural. One game that did this well was Super Mario Bros 3 - the tiles on the world
map were animated, alongside small things such as the question blocks and the tires on the tanks in the
autoscrolling levels. These subtle things made the games feel much more impressive.

The good news is, this is actually quite simple to accomplish in your game, and can be done without
using many resources! We accomplish this by switching between two (or more) sets of background tiles
(chr banks) every few frames. 

You can find an example of this in the 
[section4_animated_tiles branch](https://github.com/cppchriscpp/nes-starter-kit/compare/section4_animated_tiles) on Github.

You can also try the [example rom](https://s3.amazonaws.com/nes-starter-kit/section4_animated_tiles/starter.latest.nes).

<a href="https://cppchriscpp.github.io/nes-starter-kit//guide/section_4/animated_tiles.html" data-emulator-branch="section4_animated_tiles">
    <img alt="Test Game" src="../images/button_test-rom.png" style="margin:auto; display: block;" >
</a>

## Creating a set of animated Tiles

The first step is creating a set of animated tiles. I am making the assumption that you have already used
NEXXT to update tiles before in one of the earlier sections. (If not, you may want go go back to that 
chapter first!) Open NES Screen Tool (NESST) and open `graphics/tiles.chr`. Find the tile you want to 
animate, and change the tile (in-place) to look like a second animation frame. 

In the example branch, I opted to animate the water tile. (Note that you can animate as many tiles as
you want without cost!) **Do NOT save this to `graphics/tiles.chr`**! Instead, save it to a new file, 
named something like `graphics/tiles_2.chr`. 

If you want to see how it looks, you can load both `tiles.chr` and `tiles_2.chr` into NEXXT as Pattern
tables A and B, then switch between them. 

This chapter does not give code for showing more than two frames of animation, but adding more frames
should be fairly simple. 

## Putting them into the game

Adding this to the game won't take very long we need to add the new graphics (.chr file) to the game,
then add some C code to switch which chr file is shown every few frames. 

First, open up `graphics/graphics.config.asm`. This file maps the different chr banks
for the game to files we have created.
You should find something that looks vaguely like this: 

```asm
.segment "CHR_00"

	; We just put the ascii tiles into both sprites and tiles. If you want to get more clever you could do something else.
	.incbin "graphics/ascii.chr"
.segment "CHR_01"
	.incbin "graphics/tiles.chr"

; Note: You can put your own separate chr files here to use them... we only use 3 in the demo. This is to avoid warnings,
; and make the rom a predictable size. Note that if you do this you'll have to tweak the engine to support it! There's
; hopefully a guide on how to do this in the repo.
.segment "CHR_02"
	.incbin "graphics/sprites.chr"
.segment "CHR_03"
	.incbin "graphics/tiles.chr"
```

This will continue on to many more banks - take note of the first one to use our original tiles (`CHR_01` in this 
example). Then, add the new `tiles_2.chr` file to one of the other ones. In the example, we replaced `CHR_03`. 
Also take note of this number; we'll use both of them soon. 

Next, we need to write the code to do the switch. It's fairly simple. I'll skip over the boilerplate that
sets up banking, but you can look at it in the git branch to fill it out. The example puts this into 
`source/c/map/tile_animation.c` (with defines in the header file of the same name) Here's the method we care about: 

```c
void animate_map_tiles(void) {
    // Using any number that equates to a single binary digit (1, 2, 4, 8, 16, 32, 64, 128) should result in a constant animation.
    if (frameCount & 0x20) {
        set_chr_bank_0(CHR_ANIMATION_BANK_0);
    } else {
        set_chr_bank_0(CHR_ANIMATION_BANK_1);
    }
}
```

This method calls a method to change the chr bank used for background tiles every frame - it changes which one every
32 (`0x20`) frames. `CHR_ANIMATION_BANK_0` and `CHR_ANIMATION_BANK_1` are set to the two values we noted while editing
`graphics.config.asm`. We call this from `source/main.c` when the game is running by using the 
`banked_call` method: 

```c
banked_call(PRG_BANK_MAP_SPRITES, update_map_sprites);
banked_call(PRG_BANK_MAP_TILE_ANIMATION, animate_map_tiles);
banked_call(PRG_BANK_PLAYER_SPRITE, do_player_movement);
```

## Extending this for more than 2 animation frames

This chapter will not provide explicit code for showing more than 2 frames of animation, but setting this up should
not be too difficult. Instead of comparing against a value like `0x20` which has one binary digit (which is `00010000`
in binary), you will want to compare with a number that has two 1s next to eachother, then compare with all 4 possible
values. If you did `0x30`, you  would want to compare with: `0x00`, `0x10`, `0x20`, and `0x30` to choose which
frame to show. 