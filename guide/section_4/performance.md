# Ways to improve performance

Ok, you've got a pretty good game going, but it's slowing down when you play it! Or perhaps you just want to plan in advance for a 
game that you think will be involved. Either way, there are a number of things we can do to improve performance. The key to this is
to decide what tradeoffs you can afford.

## Option 1: Remove unused engine code

The game engine includes a lot of code to help you build your game. This includes a number of features around updating the hud, 
error checking, sprite types and collisions, and more. Most of this is intended to be useful, but some of it may be overkill, or
not used in your game. Don't be afraid to remove things you don't need! If you aren't sure, spend a little time trying to figure
out what the code in question does, then remove it and test any features you think could have been impacted/broken. Let's try
a few examples!



## Option 2: Use a less resource-hungry music engine

The music engine our game uses is pretty resource-intensive. The reason for this is that it supports all of Famitracker's features.
The goal was to allow new developers to create music without any restrictions, so they could focus more on making good music. That said,
this engine takes up _a  lot_ of cpu time, as well as a lot of ram. 

Switching to a new engine will require modifying some assembly language, a bit of neslib itself, and also likely reworking much of
your music to fit that engine's requirements. **This option is not for the faint of heart!**

## Option 3: Making code more efficient

C lets you do a lot of cool things with a lot less code than assembly language. It's very powerful, but also makes it very easy to write
inefficient things without noticing. This section covers some code that produces inefficent assembly code, and how to replace it. (Note:
this is _very_ specific to 6502 assembly, and specifically our compiler - it will not translate to other languages.)

## Option 4: Breaking logic up to run on different frames

Most of the time when we write game logic, we expect this logic to run every frame. It's simple and it works. That said, one 
often-overlooked option for improving performance is to break this habit. We can make some of our logic run on every other frame, or
even every four frames. If you can find pieces of your game that can do this and feel natural, it can get you back a lot of time. That
said, this has to be applied carefully, or it can result in weird bugs. 
