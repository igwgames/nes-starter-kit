---
title: Setting up your tools
permalink: guide/section_1/setting_up_your_tools.html
---
# Setting up your tools

You'll need a few tools to get started with NES Game development. I'll list them out, and detail what it is used for.
Make sure you have installed everything marked as **REQUIRED** before moving on to the next chapter.

## Important Note 

_This tutorial is built entirely for Windows - `create-nes-game` and the tools also support
linux on both x86_64 and arm64 systems, however other related tools may not support linux. 

This is not handled in this guide, so you will be on your own to find working tools for
graphics, etc if you use an operating system other than Windows.

## create-nes-game

**REQUIRED**

This library expects to use a tool called 
[create-nes-game](https://cppchriscpp.github.io/create-nes-game/#/)
to build the rom. This tool knows how to build C and assembly code, as well as do graphics conversions, and
a few other useful things. It can also install a copy of the code for this project.

Installing the app is recommended, however it can be run in-place if you would rather. You may have to close and
reopen any terminals you had open after installation.

## Get the code!

**REQUIRED**

The easiest way to get a copy of the code is to run `create-nes-game` in a terminal. Choose to install using a 
tutorial, then `nes-starter-kit` should appear in the list. (Be sure to use at least version 1.0.10!)

Choose that, give your game/test a name, and pick the base version of the game. Some later chapters require large
numbers of code modifications, and can be copied separately using the tool, if desired. 

If you'd rather, you can also use git to clone this repository, or download a zip from github. `create-nes-game`
will work in any of these cases. 

## Tiled

**REQUIRED**

Tiled is a tile editing tool for maps. We will be using it for designing our levels. It's similar to a paint program, but
with map tiles. We won't go into too much detail now; you can probably get by without it for a little while.

Get it here: http://www.mapeditor.org/

The tools are built for version 1.9.2, however later versions should be fine. Some earlier versions will likely work
too, but are unsupported.

## Famitracker

Famitracker is a music editing tool for the NES. We will use it to make music and sound effects for the
game. You don't have to download this right away if you don't want to; the built-in sound effects and 
music can probably cover you for a while!

## Visual Studio Code (Or other code editor)

You probably want a program to edit your code! I prefer VS Code, as it has a lot of powerful features for our use built in.

Get it here: https://code.visualstudio.com/

For best results, go to `file`, then `open folder`, and point it at your game's base folder. (Where `README.MD` is located.)

Of course, you are able to use another program such as vim or notepad++ if you prefer.

### Additional VS Code Tweaks

If you like using VS Code, there are some little things you can do to make it even better. 

#### Compile and run on pressing ctrl+shift+b

This is already set up for you too! You can change how this works by editing `.vscode/tasks.json`.

#### Suggested VS Code Extensions

These extensions aren't required at all, but add features that could make development more pleasant. 
Check out the store pages, then make the call for yourself!

##### C/C++ (Microsoft)

This provides C and C++ syntax highlighting for VS Code. You probably want this one! To note, the application will suggest
installing this automatically when you open a `.c` or `.h.` file for the first time. You don't need to find this one
manually if you don't want to.

##### MADS (Jaromir Obr)

This is a 6502 assembly language support extension. You probably don't need it for most nes-starter-kit projects.
It will provide syntax highlighting for the raw assembly language in neslib, and some of our custom extensions.
You want this if you're planning on modifying or extending the guts of `nes-starter-kit`. If you'd prefer stick with C,
you can skip this.
