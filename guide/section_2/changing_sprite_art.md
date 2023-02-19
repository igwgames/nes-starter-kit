# Changing Sprite Art

**NOTE**: _This chapter assumes you have gone through the previous chapter on adding map tiles._ 
If you haven't, take a moment to read  through it now. This builds on the knowledge in that chapter.

In the previous chapter, you learned how to edit map tiles. From doing that, you know most of what you
need to for updating sprites. There are a few differences to be aware of though, which this chapter will
go over.

To start, let's open the sprite chr file and palettes. Open `NEXXT`
and go to the `Patterns submenu` under `File`, and select `open chr`, and find `graphics/sprite.chr`. 
Next, open the `Palettes submenu` under `File` and choose `Open Palette`, then open 
`graphics/palettes/main_sprite.pal`. You should see some familiar sprites. 

![nesst sprite](../images/nesst_sprite.png)

Just as before, you can edit the images and everything else within the program, or export to a bitmap file
and edit in another program. There are a few key differences to be aware of.

## Differences from map tile editing 

First, when dealing with map tiles, we had one shared color that would appear in all map tiles. That limitation
still exists here, however in this case the color is transparent. It will show up as black (or whatever you set
it to) in the tool, but in your game, that color won't show up at all. If you are struggling with knowing which
color is which, you may want to replace the black with a bright color you don't use.


Next, the position you put the sprites into matters. We'll get into this in depth in the "Adding Enemy Sprites"
chapter, but our animation methods expect sprites to be in certain positions. 

For sprites that flip between two tiles, the two tiles should be next to each other. For an example, you can
look at the sphere at `0xca`, and its second tile at `0xcc`. You give the engine the first tile, and it
will figure out the rest.

For sprites that face 4 directions, look at the main character sprite to see how to lay it out. You essentially
want two rows: on the first, the two animations facing down, then the two animations facing up. On the second
row, you want the two animations facing right, then the 2 animations facing left. 

Sprites can be either 8x8 or 16x16 with the default engine. (You can modify the engine to support more sizes.)
8x8 sprites work the same way as 16x16 sprites, don't add any padding between the sprites. It should work the
way you would expect.

Exporting for Tiled also works almost the same way, just build your game to get a Tiled-compatible png file
built.