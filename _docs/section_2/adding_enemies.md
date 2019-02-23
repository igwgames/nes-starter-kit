---
title: Adding new enemies
permalink: guide/section_2/adding_enemies.html
---
# Adding new enemies

Adding new enemies involves a change to the list of available sprites, akin to the change to add collectible sprites.
We will add a simple enemy, and also go over how to add new movement types.

## Adding a new enemy

Let's start off strong - right now, the game has two slime sprites that you can add to your levels. To start, let's
open `graphics/sprites.chr` in NES Screen Tool. (Load up `graphics/palettes/main_sprite.pal` from the Palettes menu
too!) In addition to your slime sprites, there should be another sprite that looks like a dumb smiley face. (Graphic
courtesy the author, hence it looking silly!) 

![were gonna add a smiley](../images/add_a_smiley.png) 

There are a few things to notice about this sprite when creating your own. First, it has 4 directions, and two animation
states for each direction. The positions matter, as our code expects the sprites to be organized exactly like this. 
(That is, facing down, then facing up on one row, then on the next row, facing right then facing left) 

It is also worth noting that for animated sprites that only show two frames (don't pay attention to direction) you want
the two animation frames to be next to each other. 

Moving on, we need to add our new sprite to the list. Open up `source/sprites/sprite_definitions.c` to see the list of 
sprites. (This will be familiar if you looked at the last chapter.) You should see a couple of enemies in this list, 
easily identified by their type of SPRITE_TYPE_REGULAR_ENEMY. Here's an excerpt from that file: 

```c
const unsigned char spriteDefinitions[] = {
    SPRITE_TYPE_HEALTH, 0xea, SPRITE_SIZE_8PX_8PX | SPRITE_PALETTE_2, SPRITE_ANIMATION_NONE, SPRITE_MOVEMENT_NONE, 1, 0x00, 0x00,
    SPRITE_TYPE_REGULAR_ENEMY, 0x40, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_1, SPRITE_ANIMATION_FULL, SPRITE_MOVEMENT_RANDOM_WANDER, 0x00, 14, 0x01,
    SPRITE_TYPE_REGULAR_ENEMY, 0x80, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_2, SPRITE_ANIMATION_FULL, SPRITE_MOVEMENT_RANDOM_WANDER, 0x00, 28, 0x02,
```

Let's copy one of these guys, then customize it for our new sprite. Copy the first one, and add it to the bottom of 
the `spriteDefinitions[]` array. (Reminder: the order of lines in this array matters; don't add new ones to the 
middle!) Now, let's customize a bit. 

The second byte is the first sprite id to use. From looking in nesst, we can find the id for our smiley face is
`0x88`, so drop that in to replace the `0x40`. Our sprite is still 16x16, so we can leave that alone. The face
looks good in the blue palette, so we can leave it as `SPRITE_PALETTE_2`. `SPRITE_ANIMATION_FULL` tells the
us the sprite has 4 directions and 2 animatione frames for each, so we leave that alone too. 
`SPRITE_MOVEMENT_RANDOM_WANDER` tells our sprite to wander with collisions. 

Finally, the last 3 bytes aren't as clear... the first is a health value, where 0 is invincible. This isn't
of much value yet since we didn't give you a way to attack enemies. (Yet!) The next is the sprite's movement
speed. This is the number of subpixels to allow it to move per frame. Finding the right speed takes some 
experimentation. The last one is the amount of damage you take when the sprite hits you.  

For our use, let's make this sprite a bit faster than the slime (20 should be good) and leave it at one
heart of damage. Here's our new line after those changes: 

```c
    SPRITE_TYPE_REGULAR_ENEMY, 0x88, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_1, SPRITE_ANIMATION_FULL, SPRITE_MOVEMENT_RANDOM_WANDER, 0x00, 20, 0x01
```

If you save, then either build your game or run `make build-sprites`, you should see your new sprite available 
in Tiled. Add it to the `Sprites` layer (like we did in the 
[adding collectible sprites chapter](./collectible_sprites.html) and run the game. You're done; your 
sprite should be running around, and causing the player pain when you hit it. Grand!

![smiley ball](../images/smiley_ball.png)

## Customizing enemies further

Okay, you know how to use the built-in tools to make sprites, but what if you want to add new behavior
to sprites? You can do that too! There are a few things that can be easily tweaked.

### Adding a new animation type

We have two animation types that flip between sprites right now - one slow and one fast. Let's add one 
that is _very_ slow to demonstrate. If you open `source/sprites/sprite_definitions.h`, you will see
a list of animation types like this: 

```c
// Use this for wprites that are not animated at all.
#define SPRITE_ANIMATION_NONE 0
// This is used for sprites with two animation states; it will flip between the two.
#define SPRITE_ANIMATION_SWAP 10
// This flips sprites between two animation states, like the one above. Just faster!
#define SPRITE_ANIMATION_SWAP_FAST 11
// This is the "full" animation style - it flips between two animation frames in each direction.
#define SPRITE_ANIMATION_FULL 50
```

Let's add a new `SPRITE_ANIMATION_SWAP_SLOW` as 12. Don't forget to update 
`source/sprites_sprite_definitions.c` with this new animation value for one of your sprites! We can
use it for one of the ball sprites. 

Next, we need to update the logic that shows which sprite is which. Open up `source/sprites/map_sprites.c`,
and find the part of the `update_map_sprites()` function that references `SPRITE_ANIMATION_SWAP`. It should
look like this: 

```c
switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE]) {
    case SPRITE_ANIMATION_SWAP:

        if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
            currentSpriteTileId += ((frameCount & 0x10) >> 4);
        } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
            currentSpriteTileId += ((frameCount & 0x10) >> 3);
        }

        break;
```

There is also another `SPRITE_ANIMATION_SWAP_FAST` below this that changes the sprite speed, which works
the same way but changes a couple values. 

This works by using the `frameCount` variable, and sets a single bit on the value to 1, then does a bit
shift to switch this to 1 or 2, depending on sprite size. We can make it faster by using a larger value
for the bit, and shifting by more. Here's one way to do it: 

```c
case SPRITE_ANIMATION_SWAP_SLOW:

    if (currentSpriteSize == SPRITE_SIZE_8PX_8PX) {
        currentSpriteTileId += ((frameCount & 0x20) >> 5);
    } else if (currentSpriteSize == SPRITE_SIZE_16PX_16PX) {
        currentSpriteTileId += ((frameCount & 0x20) >> 4);
    }

    break;
```

That's it; save the game and run it, and you should see the sprite animate much more slowly. 

### Adding a new movement type

Let's say we want our smiley face to ignore physics, and walk through walls. We can make that happen!

First, let's add a new constant called `SPRITE_MOVEMENT_RANDOM_NO_COLLISION` for this sprite. Update 
`source/sprites/sprite_definitions.c` to have this instead of `SPRITE_MOVEMENT_RANDOM_WANDER` before you 
move on. Next, open up `source/sprites_sprite_definitions.h`. There should be a list of movement types
like this: 

```c
// Use this for sprites that never move.
#define SPRITE_MOVEMENT_NONE 0
// ... more here...
// This is used for sprites that wander in a randomized pattern, with collisions.
#define SPRITE_MOVEMENT_RANDOM_WANDER 3
```

Add `SPRITE_MOVEMENT_RANDOM_NO_COLLISION` under this, and give it an unused value. (Such as 4)

Now, we have to make that actually do something. The logic for this lives in `source/sprites/map_sprites.c`,
so open that up. Look for `SPRITE_MOVEMENT_RANDOM_WANDER` in the `update_map_sprites()` function. You should 
see it in a big switch statement. There is a somewhat daunting amount of code here, but don't panic; we'll 
make it make sense!

Here it is, with some the code replaced with comments:

```c
case SPRITE_MOVEMENT_RANDOM_WANDER:
    // Okay, we're going to simulate an intelligent enemy. 
    
    // First, how long have we been travelling in the same direction? Is it time for a swap?
    if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME] == 0) {
        // Yep. Figure out if direction is: none, left, right, up, or down we do this by getting a random number
        // between 0 and 8 with bit masking. If it's 0, stop for a bit... if it's 1, left... 4 down, or 5-7, maintain.
        switch (rand8() & 0x07) {
            // Bunch of logic
        }
        currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME] = 20 + (rand8() & 31);
    } else {
        --currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME];
    }

    do_sprite_movement_with_collision();

    break;
```

This logic is not extremely complicated - it tests to see how long we have been travelling in the same direction, then
if it has been a while, we update the sprite to a new direction. After this, we call the 
`do_sprite_movement_with_collision()` method, which moves the sprite in this direction, if there are no obstacles in
the way. Here's the source of that: 

```c
void do_sprite_movement_with_collisions(void) {
    // Set currentSpriteData to the sprite speed for now (NOTE: we overwrite this after the switch statement) 
    // We'll then add/subtract it from sprX and sprY
    currentSpriteData = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_MOVE_SPEED];
    switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION]) {
        case SPRITE_DIRECTION_LEFT:

            sprX -= currentSpriteData;
            if (sprX < SCREEN_EDGE_LEFT << SPRITE_POSITION_SHIFT) {
                // Roll back the position since we use sprX to place the sprite
                sprX += currentSpriteData;
                break;
            }
            
            // If we have not collided, save the new position. Else, just exit.
            if (!test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY + SPRITE_TILE_HITBOX_OFFSET)], 0) && !test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY + currentSpriteFullTileCollisionHeight)], 0)) {
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X] = (sprX & 0xff);
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X+1] = (sprX >> 8);
            } else {
                // Roll back the position since we use sprX to place the sprite
                sprX -= currentSpriteData;
            }

            break;
        // ... Same logic as above for RIGHT, UP and DOWN
    }
```

This section breaks the code up into the 4 possible directions, then tests for collision with the background. If we
hit a collision, we roll back our position by subtracting our current speed from the sprite. Otherwise, we store
our new position with the sprite, so we find ourselves in the same spot next time. This is the logic we want to change.

There are comments throughout that hopefully break it down enough to make sense. What we want to do is 
copy the whole `case SPRITE_MOVEMENT_RANDOM_WANDER` block, and paste it a second time, changing the 
`SPRITE_MOVEMENT_RANDOM_WANDER` to our  new `SPRITE_MOVEMENT_RANDOM_NO_COLLISION`. We now have our own!

Okay... so we now have a new movement type that behaves exactly like the random movement code. Let's fix
that! If you look through the code, collisions and movement are all done by the `do_sprite_movement_with_collisions()`
method, after we pick a direction. If we duplicate this method and strip out the collision code, we should get what
we want. Here's the code within the `do_sprite_movement_with_collisions()` method again:

```c
void do_sprite_movement_with_collision(void) {
    // Set currentSpriteData to the sprite speed for now (NOTE: we overwrite this after the switch statement) 
    // We'll then add/subtract it from sprX and sprY
    currentSpriteData = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_MOVE_SPEED];
    switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION]) {
        case SPRITE_DIRECTION_LEFT:

            sprX -= currentSpriteData;
            if (sprX < SCREEN_EDGE_LEFT << SPRITE_POSITION_SHIFT) {
                // Roll back the position since we use sprX to place the sprite
                sprX += currentSpriteData;
                break;
            }
            
            // If we have not collided, save the new position. Else, just exit.
            if (!test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY + SPRITE_TILE_HITBOX_OFFSET)], 0) && !test_collision(currentMap[SPRITE_MAP_POSITION(sprX, sprY + currentSpriteFullTileCollisionHeight)], 0)) {
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X] = (sprX & 0xff);
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X+1] = (sprX >> 8);
            } else {
                // Roll back the position since we use sprX to place the sprite
                sprX -= currentSpriteData;
            }

            break;

        // And the rest of the directions are here...
```

This is great, but it does a lot of work to test collisions - everything after this comment: 
```c
// If we have not collided, save the new position. Else, just exit.
```

could be skipped. Soo... let's just remove it. In each of those 4, remove the entire if/else block. The result
should look like this: 

```c
void do_sprite_movement_no_collision(void) {
    // Set currentSpriteData to the sprite speed for now (NOTE: we overwrite this after the switch statement) 
    // We'll then add/subtract it from sprX and sprY
    currentSpriteData = currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_MOVE_SPEED];
    switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION]) {
        case SPRITE_DIRECTION_LEFT:

            sprX -= currentSpriteData;
            if (sprX < SCREEN_EDGE_LEFT << SPRITE_POSITION_SHIFT) {
                // Roll back the position since we use sprX to place the sprite
                sprX += currentSpriteData;
                break;
            }
            
            break;

        // Plus the other 3 directions following the same pattern...
```

Don't forget to update the code in the `SPRITE_MOVEMENT_RANDOM_NO_COLLISION` section to call your new
`do_sprite_movement_no_collision()` method!

That should do it! Save and rebuild the game, then put the sprite onto a screen with
some walls, and you should see it go straight through!