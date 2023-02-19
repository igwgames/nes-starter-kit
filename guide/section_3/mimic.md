# Creating an enemy that mimics player behavior 

Sometimes we want an enemy that behaves uniquely, in order to make more interesting puzzles. One option is to make the
enemy mimic what the player is doing. When you walk forward, this enemy walks toward you. when you walk backwards, it
walks away. 

We will do this by adding a new enemy type, using one of our unused enemy sprites. This green ball will mirror the
player's behavior.

Follow along with this example in the git branch named
[section3_mimic_enemy](https://github.com/cppchriscpp/nes-starter-kit/compare/section3_mimic_enemy).

If you want to try it yourself, 
[download the ROM](https://s3.amazonaws.com/nes-starter-kit/section3_mimic_enemy/starter.latest.nes)

<a href="https://cppchriscpp.github.io/nes-starter-kit//guide/section_3/mimic.html" data-emulator-branch="section3_mimic_enemy">
    <img alt="Test Game" src="../images/button_test-rom.png" style="margin:auto; display: block;" >
</a>

## Getting started

Once again, we need to add a new sprite. We've done this a few times before - we need to put a new line in
`source/c/sprites/sprite_definitions.c` for our sprite. We also use a new movement type `SPRITE_MOVEMENT_MIMIC`, 
which we define in the header file. 

Here it is, at the bottom of the list:
```c
const unsigned char spriteDefinitions[] = {
// Other sprite definitions...
    SPRITE_TYPE_REGULAR_ENEMY, 0x88, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_3, SPRITE_ANIMATION_FULL, SPRITE_MOVEMENT_MIMIC, 0x01, 14, 0x01
```

If you rebuild the rom, then open the map with `Tiled`, you should see the new sprite available to be placed on the
map. (You may need to use the `Reload` option in the menu.) Make sure to do this before moving on. If you run the game 
afterwards you will see your sprite.

Next, let's make it mimic our movement!

## Adding the new movement type

In order to mimic the player's movement, we need to add code for our new `SPRITE_MOVEMENT_MIMIC` type. The code for the
current movement types is in `source/c/sprites/map_sprites.c`, so we'll add it in there. We need to look at the 
`update_map_sprites()` method, partway down. Look for the `case SPRITE_MOVEMENT_RANDOM_WANDER` text. Here's what
we do for regular moving sprites: 

```c
case SPRITE_MOVEMENT_RANDOM_WANDER:
    // Okay, we're going to simulate an intelligent enemy. 
    
    // First, how long have we been travelling in the same direction? Is it time for a swap?
    if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME] == 0) {
        // Yep. Figure out if direction is: none, left, right, up, or down we do this by getting a random number
        // between 0 and 8 with bit masking. If it's 0, stop for a bit... if it's 1, left... 4 down, or 5-7, maintain.
        switch (rand8() & 0x07) {
            case 0:
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_STATIONARY;
                break;

            // More cases ommitted... 

            default:
                // Do nothing - just carry on in the direction you're going for another cycle.
                break;
        }
        currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME] = 20 + (rand8() & 31);
    } else {
        --currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DIRECTION_TIME];
    }

    do_sprite_movement_with_collision();
    break;
```

We want to do something similar, but without the randomness. Your first thought might be to use the player's direction
in place, like this: 

```c
currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = playerDirection;
```

This "works", but the enemy sprite moves in the same direction as the player, which isn't what we want. (Note: This 
could make for a cool item to collect, though!) Instead, we need to reverse the player's direction. We also want to
test and make sure the player's moving; if the player is still, our sprite should also remain still.

As such, we need to instead pick a direction based on the opposite of the player's direction. This ends up looking
much like the code in the `SPRITE_MOVEMENT_RANDOM_WANDER` case. Here's what we end up with: 

```c
case SPRITE_MOVEMENT_MIMIC:
    // This sprite is going to just move in the same direction as the player!

    // First, is the player moving? If not, keep the player stationary.
    if (playerXVelocity == 0 && playerYVelocity == 0) { 
        currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_STATIONARY; 
    } else {
        // Otherwise, we need to figure out the player's direction and mirror it.
        switch (playerDirection) {
            case SPRITE_DIRECTION_LEFT: 
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_RIGHT;
                break;
            case SPRITE_DIRECTION_RIGHT: 
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_LEFT;
                break;
            case SPRITE_DIRECTION_UP:
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_DOWN;
                break;
            case SPRITE_DIRECTION_DOWN:
                currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_CURRENT_DIRECTION] = SPRITE_DIRECTION_UP;
                break;
        }
    }

    do_sprite_movement_with_collision();
    break;
```

If you plug this in and rebuild the game, you should find that the new sprite you added moves in the opposite direction
of the player. Great; we did it! 

## Possible extensions

### Change ths sprite speed

In the example, we used a default speed of `14` in our sprite definition. This is a bit slower than the player. This
can be changed to be a bit faster to match the player, or even to go way faster than the player. Updating the sprite
definition is all that needs to be done.

### Actually copy the player's movement

There may be some cases where you want to mimic the player's movement exactly, rather than mirroring it. The logic for
this is quite simple, and involves replacing our `switch` statement with setting the sprite's direction directly to 
`playerDirection` whenever the player's speed is non-zero.