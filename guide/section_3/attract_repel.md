# Attracting or Repelling the player

Sometimes regular movement isn't quite enough - you want something to pull the player in or push them away. It's a 
mechanic you see in many games, and it isn't that hard to implement either. We will do this by adding a new magnet
sprite that pulls the player in - we'll make it strong enough for the player to notice, but also give the player the
ability to break free of the pull.

Follow along with this example in the git branch named
[attract_repel](https://github.com/cppchriscpp/nes-starter-kit/compare/attract_repel).

If you want to try it yourself, 
[download the ROM](https://s3.amazonaws.com/nes-starter-kit/attract_repel/starter.latest.nes)

## Getting started

First thing's first, let's add a new sprite! We've done this a few times before, but we will need to add a new line
to `source/sprites/sprite_definitions.c` for our sprite. We also use a new type: `SPRITE_TYPE_MAGNET`, which we define
in the header file. There is a magnet sprite at 0xe0 that we will use for this. 

Here it is, at the bottom of the list:
```c
const unsigned char spriteDefinitions[] = {
// Other sprite definitions...
    SPRITE_TYPE_MAGNET, 0xe0, SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_2, SPRITE_ANIMATION_NONE, SPRITE_MOVEMENT_NONE, 0x00, 0x00, 0x00
```

Of note, we are actually _not animating_ this sprite in the definition, despite having two animation frames for it. The
reason for this is that we want the sprite to not animate until it is active. The plan is to have this sprite
"turn on" every few seconds. 

If you rebuild the rom, then open the map with `Tiled`, you should see the new sprite available to be placed on the
map. Make sure to do this before moving on. If you run the game afterwards you will see your sprite.

Next, we need to actually make it do something!

## Toggling the sprite animation and activity

We need to do a little special logic with magnets to make them work the way we want. Right now we don't have a great
place that we loop through all sprite types, but we do loop through all sprites for a few things in 
`source/sprites/map_sprites.c`, so we'll piggyback off of that to update the player's acceleration. We also put 
this into a section that only runs every other frame to save ourselves some time. This code lives in the 
update_map_sprites()` method. Here's the updated code: 

```c
// We only want to do movement once every other frame, to save some cpu time. 
// So, split this to update even sprites on even frames, odd sprites on odd frames
if ((i & 0x01) == everyOtherCycle) {

    switch (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_MOVEMENT_TYPE]) {
        
        // Sprite movement code is here; left out for readability
        
        case SPRITE_MOVEMENT_NONE:
        default:
            break;
    }

    // Special case for magnet to do movement and more.
    if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TYPE] == SPRITE_TYPE_MAGNET) {
        // We want the magnet to turn on/off every 128 frames
        if (frameCount & 0x80) {
            // It's on for half the time; set the sprite to be animated
            currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE] = SPRITE_ANIMATION_SWAP_FAST;
        } else {
            // It's off; set the sprite not to be animated anymore.
            currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE] = SPRITE_ANIMATION_NONE;
        }
    }
}
```

If you add this code in and rebuild the game, you will see the magnet switch on and off on a regular interval. If you
felt so inclined, you could also do this based off the player's distnace from the sprite, or have it move around by
using a different `SPRITE_MOVEMENT_TYPE` in `sprites/sprite_definitions.c`. 

Next, let's actually move the play around a bit.

## Affecting player movement

This is the main bit of this feature; we need to affect player movement whenever any magnets are active. For this we
need a couple new variables. we'll add two new variables in `source/sprites/player.c`: `playerMagnetXAccel` and
`playerMagnetYAccel` - we will update these with each magnet on the map, then apply it to the player's speed in
`player.c`. Here are the variable definitions:

```c
int playerMagnetXAccel;
int playerMagnetYAccel;
```

We'll need to also expose these variables in `source/sprites/player.h`, and also define some constants around how much
magnets affect us. We can do this by adding the following: 

```c
extern int playerMagnetXAccel;
extern int playerMagnetYAccel;

#define MAGNET_ACCELERATION_CHANGE 4
```

Next, we need to get some data into them. We'll try to calculate how much to move the player. You will be able to tweak
the speed as needed. We'll update the code we used before to update these new variables, then apply them to the 
player's movement. First, let's go back to `source/sprites/map_sprites.c` and update our logic: 

```c
// Special case for magnet to do movement and more.
if (currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_TYPE] == SPRITE_TYPE_MAGNET) {
    // We want the magnet to turn on/off every 128 frames
    if (frameCount & 0x80) {
        // It's on for half the time; set the sprite to be animated
        currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE] = SPRITE_ANIMATION_SWAP_FAST;
        
        // Set the acceleration based on the player's location compared to the magnet.
        // But only if we are not very close to the sprite already. 
        if (ABS(sprX - playerXPosition) > 4) {
            if (playerXPosition < sprX) {
                playerMagnetXAccel += MAGNET_ACCELERATION_CHANGE;
            } else {
                playerMagnetXAccel -= MAGNET_ACCELERATION_CHANGE;
            }
        }
        if (ABS(sprY - playerYPosition) > 4) {
            if (playerYPosition < sprY) {
                playerMagnetYAccel += MAGNET_ACCELERATION_CHANGE;
            } else {
                playerMagnetYAccel -= MAGNET_ACCELERATION_CHANGE;
            }
        }

    } else {
        // It's off; set the sprite not to be animated anymore.
        currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_ANIMATION_TYPE] = SPRITE_ANIMATION_NONE;
        // Also reset the acceleration for the magnet, so we don't touch the player anymore.
        playerMagnetXAccel = 0;
        playerMagnetYAccel = 0;
    }
}
```

The new piece we added does a couple things: First, it makes sure that the distance between the two sprites is 
greater than 4 units, and if so it continues to act on the sprite. Past this point, it checks which side we are on,
and adds or subtracts the acceleration constant `MAGNET_ACCELERATION_CHANGE` as a result. 

We also should be sure to reset this value before we loop over sprites each frame; otherwise we will keep increasing 
acceleration indefinitely. Add this to the very beginning of `update_map_sprites()` in the same file: 

```c
void update_map_sprites() {
    lastPlayerSpriteCollisionId = NO_SPRITE_HIT;
    
    // Reset the player's magnet acceleration, so we are only affected by magnets active this frame.
    playerMagnetXAccel = 0;
    playerMagnetYAccel = 0;

    // Rest of the method...
```

With that done, all we need to do is make the player move based off of this. The logic to do this isn't too complex. 
Pop open `source/sprites/player.c` and let's add a little more logic to the `hanle_player_movement()` method. This is
actually only two lines, which we add after we have finished updating movement based on the player's input: 

```c
    playerXVelocity += playerMagnetXAccel;
    playerYVelocity += playerMagnetYAccel;
```

After adding this in, if you go to the tile you added the magnet to, you should see it start to pull you when the magnet
is on. Success!

## Taking it further

This is a very simple implementation, but you can definitely take it a bit further if you want. There are a number
of ways you could improve this. 

### More magnets!

There's no reason for you not to include multiple magnets on a screen, with no code changes needed. Since we set the
acceleration to 0 each frame, then add/subtract from the player's speed, multiple magnets should work fine.

### Tweak the speed

The acceleration applied via the magnet is handled by the `MAGNET_ACCELERATION_CHANGE` variable added earlier on - the
value of this dictates how hard the magnet pulls the player. Higher values mean faster pull. 

### Make the magnet repel the player instead

The magnet looks at the player's location, and pulls the player closer based on the difference between their two
positions. If we simply reverse the if statements in `source/sprites/map_sprites.c` to check if 
`playerXPosition > sprX` instead of `playerXPosition < sprX` (and the same with `sprY`) we can reverse the direction.

### Only pull the player when close to the magnet

Another option is to only pull the player when they are within a certain distance of the magnet. We do a test now on
the absolute value of the distance between player and sprites, and don't move the player if they are within 4 distance
units of the magnet's position. We can use this same logic to test how far away the player is. Here is a rough 
example of the code for that: 

```c
if (ABS(sprX - playerXPosition) < 180 && ABS(sprY - playerYPosition) < 180) {

    // Existing code
    if (ABS(sprX - playerXPosition) > 4 ) {
        if (playerXPosition < sprX) {
            playerMagnetXAccel += MAGNET_ACCELERATION_CHANGE;
        } else {

    // The rest of this logic stays the same...
```

### Move the magnet itself

The magnet is a regular sprite; there is no reason not to make it move like some of our other sprites. You can change
`SPRITE_MOVEMENT_NONE` to `SPRITE_MOVEMENT_LEFT_RIGHT` or even `SPRITE_MOVEMENT_RANDOM_WANDER` in the sprite's 
definition. (`source/sprites/sprite_definitions.c`)

### Make the magnet into an enemy

With a few changes, you could also make the magnet into an enemy. You would need to find out where the
`SPRITE_TYPE_MONSTER` type triggers damage, then alter this check to also look at `SPRITE_TYPE_MAGNET`. 

### Add a sound effect when the magnet is on

This is covered in other chapters, but adding a little sound when the magnet kicks off should give the player an extra
notification that something is happening.