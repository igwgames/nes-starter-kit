# Changing Movement

How the player moves is a big part of how your game feels. nes-starter-kit tries to pick reasonable defaults, but if the
movement is too fast or slow, it's not hard to change. In addition, you may want to make your character move faster in
certain circumstances -- such as when you press a button or get a powerup. This chapter should help!

## Adjusting the player's base speed

Your player's speed is mainly controlled by a few constants, as well as some logic in the player sprite file. 
To adjust your base speed, we simply have to tweak these constants. To start, open up `source/sprites/player.h`.
About halfway through the file, you should see something like this: 

```c
// Handles player movement - we start slow and accelerate/decelerate up to a max speed. As a player,
// you probably won't notice this outright, but it feels a lot more natural like this.
// NOTE: The max velocity needs to be evenly divided by the acceleration. (Eg 20/4 = 5)
#define PLAYER_MAX_VELOCITY 20 // Max speed of the player; we won't let you go past this.
#define PLAYER_VELOCITY_ACCEL 4 // How quickly do we get up to max velocity? 
```

These two constants are what we have to change. Make sure to read and understand the `NOTE` message here -
if these two variables are adjusted in a way where acceleration does not divide cleanly into the max 
velocity, the player will move erratically. 

Let's speed the player up a bit - let's add two more increments to `PLAYER_MAX_VELOCITY` (bringing it to 28),
then try the game to see how it feels. 

```c
#define PLAYER_MAX_VELOCITY 28 // Max speed of the player; we won't let you go past this.
#define PLAYER_VELOCITY_ACCEL 4 // How quickly do we get up to max velocity? 
```

If you rebuild the game, the player's movement should feel a bit faster. Now, let's explore the 
acceleration a bit. We can slow it down by reducing `PLAYER_VELOCITY_ACCEL` to 2. (Which still
evenly divides 28, so it is ok.)

```c
#define PLAYER_MAX_VELOCITY 28 // Max speed of the player; we won't let you go past this.
#define PLAYER_VELOCITY_ACCEL 2 // How quickly do we get up to max velocity? 
```

This should make the player's controls a bit less responsive - the player takes longer to get
to their max speed, then decelerates more slowly as well. It will feel sort of like ice physics in 
some games. 

Before moving on, adjust these variables back to something that seems right to your game. You can use
the built-in defaults, or something a little faster if you like.

## Adding a "run" button

Next, let's adjust some of these things during gameplay! As an example, we are going to adjust your speed when a button
is pressed. That said, you should be able to tweak this code to run in other cases, such as when a powerup is picked up 
for a short time.

Let's start by adding a second max speed constant to `source/sprites/player.h` for run speed. Make sure that both speeds
can be evenly divided by PLAYER_VELOCITY_ACCEL! 

```c
#define PLAYER_MAX_VELOCITY 20 // Max speed of the player; we won't let you go past this.
#define PLAYER_MAX_RUNNING_VELOCITY 36 // Max speed of the player when running.
#define PLAYER_VELOCITY_ACCEL 2 // How quickly do we get up to max velocity? 
```

Next up, we have to change how we use these constants a bit. Open up `source/sprites/player.c` and look for the 
`handle_player_movement()` method. At the top of it, you should see a familiar constant being used. 

```c
void handle_player_movement(void) {
    // Using a variable, so we can change the velocity based on pressing a button, having a special item,
    // or whatever you like!
    int maxVelocity = PLAYER_MAX_VELOCITY;
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }
```

All that we need to do is alter `maxVelocity` based on whether or not the player is running. The code is set
up to automatically shift the player's speed up or down based on maxVelocity. We just need to change it when B
is pressed... here's what it looks like: 

```c
// Put this under the if statement for PAD_START
    if (controllerState & PAD_B) {
        maxVelocity = PLAYER_MAX_RUNNING_VELOCITY;
    }
```
That's really all there is to it. Rebuild the game and run it, and when you hold B you should move a lot faster
than without. 

## Adjusting the player's hitbox

One other thing that matters is the player hitbox. If you change the player sprite at all, you may find that parts
of it overlap the walls, or perhaps don't get as close to the walls as you expect. You might also find that sprite
collisions are too big. If so, the reason is your hitbox.

If you aren't familiar with the concept, a hitbox is like an imaginary rectangle around your sprite that is used
for collisions. We do this because testing each pixel is far too slow, as are many other methods. This is a quick
and simple method to implement, and also easy to visualize and understand.

Our case is a little special though - we calculate position and size using larger numbers than the number of pixels
shown on the screen. Our sprites are 16 pixels wide and 16 pixels tall, and the screen is 256 pixels by 240 pixels.
We use larger numbers to make movement more fluid - so you can move 1.5 pixels per frame, for example. In our case,
the main sprite is 256 units wide, and 256 units tall. The screen is actually 4096 units by 3840 units. 

Here are the values that make up the hitbox of the main character - we cut a number of units off on all 4 sides, just
to make collisions feel more accurate. The constants for this are in `source/sprites/player.h`. Look for the following:

```c
// This is the width and height of player as used in collisions. This is shifted by 4 to allow for sub-pixel
// sizes. (And smoother acceleration/deceleration.) A 16px wide sprite is 256 units here.
#define PLAYER_WIDTH_EXTENDED 205
#define PLAYER_HEIGHT_EXTENDED 240
#define PLAYER_X_OFFSET_EXTENDED 28
#define PLAYER_Y_OFFSET_EXTENDED 16
```

This basically means we're starting your hitbox 1 pixel below the main sprite, and ending it at the bottom of the
sprite itself. We're cutting just shy of 2 pixels off the left of the sprite, and a little bit less on the right. 
You can change these values to suit any different character sprites you add.

As an example, let's dramatically shrink the player's hitbox and see what it does. Let's pretend our player sprite
is 10 pixels by 10 pixels, in the middle of the sprite. (So, 3 pixels of padding on all sides. Here is what that
could look like: 

```c
#define PLAYER_WIDTH_EXTENDED 160
#define PLAYER_HEIGHT_EXTENDED 160
#define PLAYER_X_OFFSET_EXTENDED 48
#define PLAYER_Y_OFFSET_EXTENDED 48
```

If you build your game and run it like this, you should see the player sprite starts to phase through objects. (Unless
you take the time to make a 10x10 sprite!) Hopefully this helps make it a little clearer.

With that, you've made it through chapter two! (Or at least this part of it.) Next up, we'll start adding some new
features in. 