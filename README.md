# NES Starter Kit
 A Beginner's Guide and toolkit for NES game creation.

## **Work in progress!!** 

Fair warning, this project is extremely work in progress. This readme tries to outline the end product, but please be
warned **this is not anywhere near a finished project yet**. Imagine this like the "Under Construction" pages on
Tripod and Geocities back in the day ;)

That said, if you decide to check this out and have thoughts, please share them. You are welcome to open an issue or PR, 
or even just send me a Tweet/DM. You can help shape the outcome of this project.

## What is this?

A fully-fledged starter kit for NES games. It is the source to a top-down adventure game, alongside all of the tools
needed to change it. It also comes with a guide to changing everything about it, alternatively titled
"Making an NES Game for Dummies".

The idea is, download this (either via git or github zip file), set up the tools, run the game, then tweak it and make
your own! With the right hardware, you can even play your game on an actual NES.

The only thing this is *not* is an introduction to programming - if you have never programmed anything before, I
suggest finding a tutorial online for C or C++, and getting a couple really basic programs working. (At the
very least, find out what a "hello world" program is, and write+run one successfully.) 

TODO: Find a good tutorial that uses a decent modern compiler. Don't let this scare people away!!


## Related projects

There are a few other projects worth knowing about that might be a better fit for your task.

First off, there's the upcoming NESMaker project. This has not yet been released, but it is a full-blown
gui for building games. Check out the [NESMaker website](http://www.thenew8bitheroes.com/)! This project is 
a good fit if you don't quite want to get your hands dirty with code.

In a similar vein, if you'd like a way to do that with Gameboy games, look into my ClassicGB project. It's a 
code-free web-based game builder. It also exports C source code, in case you want to extend it yourself! Check
that out on [the ClassicGB website}(http://game-builder.cpprograms.net)!

Lastly, if you want something more barebones, I also have nes-c-bootstrap - it is a simpler "Hello World"j
project that you can use for more of a blank page. That project is elsewhere 
[on Github](https://github.com/cppchriscpp/nes-c-boilerplate).

# Table of Contents

This guide is broken into 4 main chapters: 

1. Getting Started
2. Making it Your Own
3. Polish
4. Advanced topics.

The suggested approach is to follow along with all of part 1 in order to get yourself familiar with
the console/tools/etc. After that, feel free to jump around to whatever chapters interest you. 

The second part mainly focuses on core things you will want to change to make your own game. Art, music
gameplay and more!

The third part focuses on more cosmetic things - don't be afraid to get into this chapter early; 
sometimes adding a little bit of polish can really help encourage you to continue working on the game.

The fourth part deals with more advanced topics - for most simple games you should not need to get into
the topics in this section. That said, these topics are here if you need them, or for that matter if they
interest you.

TODO: Links once these exist

### Part 1: Getting started

1. Setting up your tools
2. Building your game
3. Playing your game
4. Making Your first change
5. FAQ

### Part 2: Making it your Own

6. Changing the overworld map
6. Adding new map tiles
7. Changing the sprite art
8. Adding new enemies
9. Adding collectible sprites
10. Adding/changing music
11. Adding/changing sound effects
15. Changing movement


### Part 3: Polish and more!

20. Ways to improve performance 
    (Note: these will likely include changing the music engine, and doing stuff every other frame)
21. Making the title screen prettier
    (Note: Probably means showing how to replace the text title screen with something prettier)
22. Playing your game on a real NES
23. Making a "real" cartridge

### Part 4: Advanced Topics

_Things in this section are somewhat complicated, and require a good knowledge of how the NES works. You shouldn't
need these things for most games, but if you do, this is a place to learn!_

30. Understanding and tweaking the build tools
31. Understanding and adjusting the size of your game
32. ROM Data Map
33. Expanding available graphics using CHR banking
34. Making room for more code using PRG banking
35. Getting finer control over graphics with chr ram
36. Writing Assembly Subroutines

# Credits

[NESLib](https://shiru.untergrund.net/software.shtml) by [Shiru](http://shiru.untergrund.net/)

[Early 90's Arcade Pixel Art](https://opengameart.org/content/early-80s-arcade-pixel-art-dungeonsslimes-walls-power-ups-etc) 
by [Refresh Games](https://refreshgames.co.uk/)

[Who's Cuter Font](https://forums.nesdev.com/viewtopic.php?f=21&t=10284&start=0) by [Damian Yerrick (Tepples)](http://pineight.com)

**Please credit these wonderful people in your own creations, too!**

# Contributing

I don't have any set rules around this right now. If you've got something you'd like to add, just send over a pull 
request, and I'm sure we can get something together.


# Rough notes for my use
- Will likely want to use famitracker directly over famitone2 to lower the barrier of entry
- Should probably target windows explicitly - need to either not require GOW, or make it very explicit. The simpler the 
  better. At the same time though, makefiles are absurdly powerful... food for thought.
- Do we want to give the player a sword/something similar? Or can we get by not doing that?
- Improve the banking script to actually have the cache
- Add an error screen, and use it from banking script. (More places?)
- Pretty fade in/out methods
- Add music early.
- Add contact info to title && error screen (error screen, include developer name too)?
- Stretch: Take chr, take palettes, combine into the 4-tile tall png we know and love and/or individuals.
- How can we map which tiles are solid vs not? Needs to be super obvious and user-editable...