# Extending the available graphics via CHR banking

In looking at the graphics you are able to create in NES Screen Tool, you likely have realized they are very
limited. You have 256 8x8 tiles on each of the two maps available, and where our tiles/sprites are often 
16x16, this leaves us with 64 actual tiles per map. You can fit a maximum of 7 full sprites on a single 
map. (Including the player!) How can we work around these restrictions?

The answer is chr banking, as you might have guessed. We actually already used this in the chapter on
[background tile animation](../section_4/animated_tiles.md). The same general approach applies here, but
we will try to outline strategies to use it for more complex cases. 

In short, chr banking allows us to switch out sets of background/sprite tiles in code very quickly during
gameplay. We can only switch a full tileset (256 tiles) at once, so we likely need to duplicate tiles between
these various files. Note that this is only a graphical change; all sprite/background logic needs to be
written to support this.

There is no example code/branch for this chapter; if you can see a good way to prove this out, please feel
free to submit a PR to the project with a git branch demonstrating it; I'd be happy to extend this!

## Creating more graphics 

Adding more graphics to your rom is actually fairly easy - there is just a file that contains a list of
chr files, and we can add ours to that list. First of course, you need to make a new chr file using NES
Screen Tool. Creating graphics with NESST has been covered in other chapters, so I am not going to detail 
that here. I will detail some things to keep in mind for background and sprite layers. These are meant for 
regular gameplay; if you are creating a menu/splash screen/etc, these likely do not apply.

One thing to keep in mind is that once you start splitting your graphics into multiple chr files, you end
up duplicating a number of tiles for the HUD, main character, and the like. This is fine, but you have to
_remember to update all chr files when you change these graphics_! This is extremely easy to forget, and can
lead to confusing bugs. 

In my personal projects, I tend to export all of my graphics from NES Screen Tool into bitmap files, then
update those bitmap files and re-import them. Unfortunately this is time consuming and error-prone. There
is likely a better way. (PRs and suggestions are welcome!)

### Notes about adding tile/background graphics

If you are going to add new tile/background graphics, you will want to be sure you keep the tiles that are
used throughout the game in all created tilesets. In the example, I have tried to keep this to the last two 
rows of 8x8 tiles in `graphics/tiles.chr`. You can generally create a new tileset and keep this bottom row,
while blowing everything else away and replacing it. (This contains most of the things that make up the HUD
at the top) Keeping these tiles in the same place also makes things much easier, and is strongly recommended.

Also keep in mind the way we split tiles that are solid vs ones that aren't. If you have not modified anything,
the top 3 rows contain tiles that can be walked on, and the 3 rows below it are solid blocks. Your best bet
is going to be maintaining the same rules for any new tiles you create.

### Notes about adding sprite graphics

Creating sprite graphics has a couple important things to keep in mind. First of all, every sprite tileset
needs to have the main character in it. You will also need any shared sprites (such as hearts, keys, and
any other drops that you can have anywhere.) As with tiles, keeping them in the same place is highly
recommended to keep the code simple. 

## Putting the new graphics into your rom

Once you have created new chr files for your tiles or sprites, it is a simple matter of adding these to the
list of chr files available to the ROM. You can add up to 32 chr files - 3 are used by the engine by default.
Open up `source/neslib_asm/crt0.asm` (The entrypoint for C) and scroll towards the bottom. You should see some
code that looks like this: 

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
.segment "CHR_04"
	.incbin "graphics/tiles.chr"
; A lot more of these...
```

The first 3 (`CHR_00`, `CHR_01`, and `CHR_02`) are used by the engine as it exists. Everything after this is open 
for your new chr files. Just replace `graphics/tiles.chr` with the name of your new file. Also keep track of which
slot you put it into, as you will have to use this to tell the code to switch to this new bank. (You probably want
to put it in a constant, with another constant for the `chr` file you're supplementing.)

## Switching to use these graphics

The act of switching to use a new chr file is actually very simple, and can be done with one command. If you want 
to try switching to your new bank, you can simply change the code in the `init_map()` method in `source/map/map.c`
to use your new chr bank. Look for this code: 

```c
    // Make sure we're looking at the right sprite and chr data, not the ones for the menu.
    set_chr_bank_0(CHR_BANK_TILES);
    set_chr_bank_1(CHR_BANK_SPRITES);
```

Simply replace `CHR_BANK_TILES` or `CHR_BANK_SPRITES` with your new bank id, and you're in business. You can also
call these methods at any time during gameplay to switch things. The animation chapter does exactly this to animate
the background tiles we use.

The complication here is tooling - the tooling built into this kit expects us to only have one set of tiles and one
set of images for sprites. It can certainly be done, but it will take a little bit of work.

## Using a second set of map tiles

Using a second set of map tiles should be achievable if your desire is to simply use them with a separate map. This
could build off of the [second map chapter/Github Branch](../section_3/adding_a_second_map.md) to have a different
set of tiles for the different supported maps. 

First, each map would need to generate its own png file to show map tiles. This should be pretty easily achievable
by modifying the `makefile`. You would want to change two rules. First, the build-tiles rule should build two images:

```makefile
# Before: 
build-tiles: graphics/generated/tiles.png
# After:
build-tiles graphics/generated/tiles-a.png graphics/generated/tiles-b.png
```

The second makefile change is to the tile png generator itself: 

```makefile
# Before: 
graphics/generated/tiles.png: graphics/tiles.chr graphics/sprites.chr graphics/palettes/main_bg.pal
	$(CHR2IMG) graphics/tiles.chr graphics/palettes/main_bg.pal graphics/generated/tiles.png
# After: 
graphics/generated/tiles-a.png: graphics/tiles-a.chr graphics/sprites.chr graphics/palettes/main_bg.pal
	$(CHR2IMG) graphics/tiles-a.chr graphics/palettes/main_bg.pal graphics/generated/tiles-a.png
graphics/generated/tiles-b.png: graphics/tiles-b.chr graphics/sprites.chr graphics/palettes/main_bg.pal
	$(CHR2IMG) graphics/tiles-b.chr graphics/palettes/main_bg.pal graphics/generated/tiles-b.png
``` 

From there, you just have to update Tiled to look at `tiles-a.png` for the first map, and `tiles-b.png` 
for the second map. (Note: You can also pass in a separate palette here; just be sure to also set the
palette when you set the tiles in code.)

Now you have your map editor showing the second tileset for your second map. We just have to update the
C code to change to the new map whenever it loads a new map screen. This logic lives in `source/map/map.c`,
in the `draw_current_map_to_nametable` method. We can sneak some new logic in here to pick a chr tile id
based on the current map id. (The screen will be off when this method is called.) Here's an example
of that code being added in: 

```c
draw_current_map_to_nametable(int nametableAdr, int attributeTableAdr, unsigned char reverseAttributes) {

    // Switch to the correct CHR bank: 
    if (currentWorldId == WORLD_OVERWORLD) {
        set_chr_bank_0(CHR_BANK_OVERWORLD);
    } else {
        set_chr_bank_0(CHR_BANK_UNDERWORLD); 
    }

    // Prepare to draw on the first nametable
    vram_inc(0);
    set_vram_update
```

Note that if you are also using tile animation, you will have to update that logic to also know about
both of these tiles. If you did this following the example, that logic lives in `source/map/load_map.c`. 

## Tips for using a second set of sprite tiles

This isn't something this author has attempted, but it should be possible. The challenge with this is that
we have a lot of logic attached to our sprites and their ids, so it is much harder to write logic that 
behaves correctly depending on which graphics are loaded.

One easy way to do it might be to have different "skins" for the same set of X sprites in chr files, 
then choose which chr file to use based on the map id. You could use the same type of change we did
to the `makefile` in the Background Tile section to make things look nicer in Tiled.

To create completely separate sprites, I suspect you would need to have multiple copies of the methods
in `source/sprites/map_sprites.c`, and multiple sets of `sprite_definitions` that would be read depending
on which set is loaded. If you manage to do this in an easily understandable way, please reach out! I'd
be interested in documenting a good way to do this.