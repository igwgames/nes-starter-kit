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
a good fit if you don't quite want to get your hands dirty with code. You may be able to edit the resulting
code, but it will likely be in assembly language, and a little harder to pick up if that is new to you.

In a similar vein, if you'd like a way to do that with Gameboy games, look into my ClassicGB project. It's a 
code-free web-based game builder. It also exports C source code, in case you want to extend it yourself! Check
that out on [the ClassicGB website}(http://game-builder.cpprograms.net)!

Lastly, if you want something more barebones, I also have nes-c-bootstrap - it is a simpler "Hello World"j
project that you can use for more of a blank page. That project is elsewhere 
[on Github](https://github.com/cppchriscpp/nes-c-boilerplate).

## Feature Comparison

| Feature Name           | nes-c-starter-kit  | NESMaker           | gameboy-bootstrap  |
|------------------------|--------------------|--------------------|--------------------|
| Language               | C                  | 6502 Assembly      | C                  |
| Gui Editing            | :x:                | :heavy_check_mark: | :heavy_check_mark: |
| Game Style             | Adventure only     | Many               | Adventure only     |
| Graphics editor        | external - nesst   | Built-in           | Built-in           |
| Map Editor             | external -tiled    | Built-in           | Built-in           |
| Music Editor           | ext - famitracker  | ext - famitracker  | Not available      |
| Sound Editor           | ext - famitracker  | ext - famitracker  | Not available      |
| Works on console       | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Source available       | Yes, you write it! | _Unknown_          | Yes                |
| Source easily editable | Yes, built for it  | _Unknown_          | No                 |
| Code Editor available  | Yes, VS Code       |_Unknown_ (unlikely)| No                 |
| Programming required   | Yes                | No                 | No                 |

**Note**: NESMaker was not available at the time of this writing, so some of these details are based on
          speculation. If something's wrong, submit a PR!

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

The third part focuses on how you can extend the game further. Want to give the main character a weapon?
Add some new features to the start menu? Do something else unique? This has guides and pointers on how to
do it!

The fourth part focuses on more cosmetic things - don't be afraid to get into this chapter early; 
sometimes adding a little bit of polish can really help encourage you to continue working on the game.

The final part deals with more advanced topics - for most simple games you should not need to get into
this chapter. It gets into depth on details of how the library itself works, as well as the system, and how
the rom is organized. That said, these topics are here if you need them, or for that matter if they
interest you.

TODO: Links once these exist

### Part 1: Getting started

1. [Setting up your tools](./guide/section_1/setting_up_your_tools.md)
2. [Building your game](./guide/section_1/building_your_game.md)
3. [Playing your game](./guide/section_1/playing_your_game.md)
4. [Making Your first change](./guide/section_1/your_first_change.md)
5. [FAQ](./guide/section_1/faq.md)

### Part 2: Making it your Own

10. [Changing the world](./guide/section_2/changing_the_world.md)
11. Understanding art on the NES
12. [Adding new map tiles](./guide/section_2/adding_new_map_tiles.md)
13. Changing the sprite art
14. Changing the HUD (top bar)
15. Adding new enemies
16. Adding collectible sprites
17. Adding/changing music
18. Adding/changing sound effects
19. Changing movement

### Part 3: Adding new features

_(Note: these are rough ideas and very likely going to change. Suggestions also welcome)_
20. Giving your main character a sword
21. Adding more features to the pause menu
22. Adding a second map
23. Making the main character grow or shrink
24. Adding objects that attract or repel the player
25. Adding an enemy that mimics player behavior

### Part 4: Polish and more!

30. Ways to improve performance 
    (Note: these will likely include changing the music engine, and doing stuff every other frame)
31. Making the title screen prettier
    (Note: Probably means showing how to replace the text title screen with something prettier)
32. Animating background tiles
33. [Playing your game on a real NES](./guide/section_4/real_nes.md)
34. Making a "real" cartridge

### Part 5: Advanced Topics

_Things in this section are somewhat complicated, and go into how the engine works. You shouldn't
need these things for most games, but if you do, this is a place to learn!_

40. Understanding and tweaking the build tools
41. Understanding and adjusting the size of your game
42. [ROM Data Map](./guide/section_5/rom_map.md)
43. Expanding available graphics using CHR banking
44. Making room for more code using PRG banking
45. Getting finer control over graphics with chr ram
46. [Writing Functions in Assembly](./guide/section_5/writing_assembly_functions.md)

# Credits

[NESLib](https://shiru.untergrund.net/software.shtml) by [Shiru](http://shiru.untergrund.net/)

[Early 90's Arcade Pixel Art](https://opengameart.org/content/early-80s-arcade-pixel-art-dungeonsslimes-walls-power-ups-etc) 
by [Refresh Games](https://refreshgames.co.uk/)

[Who's Cuter Font](https://forums.nesdev.com/viewtopic.php?f=21&t=10284&start=0) by [Damian Yerrick (Tepples)](http://pineight.com)

[8-Bit Victory Loop](https://opengameart.org/content/8-bit-victory-loop) by [Wolfgang](https://opengameart.org/users/wolfgang)
[8-Bit Forest Theme](https://opengameart.org/content/8-bit-forest-theme) by [Wolfgang](https://opengameart.org/users/wolfgang)

**Please credit these wonderful people in your own creations, too!** Technically all of the art and music is licensed
as public domain, (CC0) but it'd be really nice to give them credit if you don't replace it.

# Contributing

I don't have any set rules around this right now. If you've got something you'd like to add, just send over a pull 
request, and I'm sure we can get something together. Just try to follow the existing code style as best you can.


# Rough notes for my use
- Will likely want to use famitracker directly over famitone2 to lower the barrier of entry
- Do we want to give the player a sword/something similar? Or can we get by not doing that?
- Add some famitracker sfx
- Add contact info to title && error screen (error screen, include developer name too)?
-- But doing this actually seems way less fun, looking at it... we want to get the user curious
   with their first change!
- Better document how doing a fork works, and more importantly how to upgrade
- Slightly nicer pause menu?
- better character hitbox