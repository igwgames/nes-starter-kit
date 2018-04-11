# Adding a new collectible sprite

Adding new sprites involves changing our list of available sprites, then adding some logic to handle the new sprite.
We will start simple with a new heart object, then expand on this to make it a new thing.

TODO: Do we update the hud with the new thing?

## Adding the new sprite object

First, let's create a new sprite. Open up `source/sprites/sprite_definitions.c`, and you should be able to see a list
of existing sprites, with one sprite per line. It looks something llke this:

```c
const unsigned char spriteDefinitions[] = {
    SPRITE_TYPE_HEALTH, 0xea, SPRITE_SIZE_8PX_8PX | SPRITE_PALETTE_2, SPRITE_ANIMATION_NONE, SPRITE_MOVEMENT_NONE, 1, 0x00, 0x00,
// ... a bunch of additional sprite definitions here...
    SPRITE_TYPE_ENDGAME, 0xe8, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_1, SPRITE_ANIMATION_NONE, SPRITE_MOVEMENT_NONE, 0x00, 0x00, 0x00

};
```

In particular, we are interested in that first one, which is the heart sprite we can pick up to
increase our health. Let's make a new one! Copy that line at the top, and paste it at the bottom as
a new sprite. Be sure to add a comma on the line above your new one to avoid syntax errors. You will
also need to remove the comma at the end. 

Next, let's customize it a little. First, let's change the color! The current sprite uses palette 2, 
which is mostly red. This makes sense for a regular heart, but we want a super-heart, so we will use
the blue color palette. Try changing `SPRITE_PALETTE_2` to `SPRITE_PALETTE_1` - it's blue now! 

Let's make sure we can see the blue sprite next. First, we need to update the sprites that Tiled
knows about. You can run `make build-sprites` to do this instantly. (Alternatively, this is built
every time you build the game. You can just build the game instead.) If you open Tiled, you should
see the new sprite available in the `Sprites` tab. If not, try closing Tiled and reopening it. 

Once you see it, we need to add the Sprite to your map. Be sure to add it to the `Sprites` layer
instead of the map layer, or it will not work. If you then rebuild your game, you should see the
new sprite on the map. If you try to collect it, you will get one heart. 

![Blue heart sprite](../images/blue_heart.png)

## Making the sprite give us extra health

Alright, we have a sprite that gives us health back now, but let's customize it a little bit
more. The engine has some logic to heal the player based on the number in the first byte
reserved for miscellaneous data. (Byte 6) It is set to 1 in the first definition. Let's
up that to 2 instead! 

Open `source/sprites/sprite_definitions.c` again, and find the line you added for the blue
heart. The 6th byte should be set to 1 - change this to a 2 and save the game. Now rebuild
the game, and collect your blue heart. You should heal two hearts! Let's dig into why.

The resulting C code in `sprite_definitions.c` should look like this:
```c
SPRITE_TYPE_HEALTH, 0xea, SPRITE_SIZE_8PX_8PX | SPRITE_PALETTE_1, SPRITE_ANIMATION_NONE, SPRITE_MOVEMENT_NONE, 2, 0x00, 0x00
```

Open up `source/sprites/player.c` and look for a function called `handle_player_sprite_collision()`.
This method is called any time the engine determines that the player and a sprite are in
the same space. This function handles the sprite's response, as well as the player's. 

For example, if  the sprite is an enemy, it pushes the player back and takes away some 
health. In our case though, it adds some health back. There is a switch statement in here 
that goes through sprite types - we are concerned with `SPRITE_TYPE_HEALTH`.

Below the type, there is some code that changes the player health, and also removes
the sprite from the screen. You can tweak this code to heal the player more, or
never make the sprite go away if you want. We'll expand on this to make our own
new type of collectible sprite!

## Changing the sprite behavior completely