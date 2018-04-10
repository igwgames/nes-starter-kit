# Setting up your tools

You'll need a few tools to get started with NES Game development. I'll list them out, and detail what it is used for.
Make sure you have installed everything marked as **REQUIRED** before moving on to the next chapter.

## Get the code!

**REQUIRED**

First, if you haven't already done so, get a copy of this repository. You may do this either by doing a `git clone` of
this repository, or downloading it as a zip file. 

To download a zip file, go to this project's page on Github, and click the green `Clone or download` button at the top right.

You should also rename the folder from `nes-starter-kit` to the name of your game!

## Base tool zip

**REQUIRED**

To make life easier, most of the tools, including the compiler, c library, and various converters are 
included in the following zip file: 

TODO: Upload this zip!! 

Download this file, and extract it to the "tools" folder. Once you have done that, follow the steps below.

**NOTE**: _This tutorial is built entirely for Windows - while things may be compatible with other 
Operating Systems, I don't want to spend extra time supporting them._

## Cygwin, make, and unix tools

**REQUIRED**

These are some tools usually available on unix-based systems for building things. We mainly need the `make` command. 
You won't use this directly at all, but it is needed to build your NES rom.

To install this, go to the tools folder, and double-click `install_cygwin.bat`. This will download and launch the installer
for cygwin. You will need to go through the entire wizard - it just pre-selects some packages for you. Pick any url for the 
mirror when you get to that step; it does not matter.

**NOTE**: _The script isn't strictly necessary, but it selects the packages we need automatically, in addition to the defaults.
It is provided merely as a convenience. If you have cygwin installed already, make sure you have `make`, `wget` `chere`, and `curl`
installed._ Alternatively, the `install_cygwin.bat` script won't delete/change your existing packages, so you can just use that.

## FCEUX (Or another NES Emulator)

**REQUIRED**

You will need an NES emulator to test your game! I typically use FCEUX, but you are welcome to use whatever you are comfortable
with. Nintendulator is another common option.

FCEUX is available here: http://www.fceux.com/

Next double click a rom in Windows explorer, then select your emulator to open it with once. This allows
the built-in tools to open your emulator of choice easily.

First, choose the "Select a program from a list of installed programs" option:

![Step 1](./images/fceux_1.png)

Make sure the "Always use the selected program to open this kind of file" checkbox is ticked, then click browse, and find the
emulator you downloaded: 

![Step 2](./images/fceux_2.png)

(Note: On Windows 10 you have to click `more apps`, scroll down and find `look for another app on this pc` instead.) 

If you don't do this, running your rom will not work.

## Tiled

**REQUIRED**

Tiled is a tile editing tool for maps. We will be using it for designing our levels. It's similar to a paint program, but
with map tiles. Just install it for now; we will talk about it when we start editing maps.

Get it here: http://www.mapeditor.org/

The tools are built for version 1.1.1, however later versions should also be fine.

## Visual Studio Code (Or other code editor)

You probably want a program to edit your code! I prefer VS Code, as it has a lot of powerful features for our use built in.

Get it here: https://code.visualstudio.com/

For best results, go to `file`, then `open folder`, and point it at your game's base folder. (Where `README.MD` is located.)

Of course, you are able to use another program such as vim or notepad++ if you prefer.

### Additional VS Code Tweaks

If you like using VS Code, there are some little things you can do to make it even better. 

#### Set built-in terminal to cygwin

If you have installed cygwin with the built-in script, this should just work. 

If you used a 32 bit cygwin, or did not install it in the default directory, you will have to tweak things slightly. 
Open `nes-starter-kit/.vscode/settings.json`, and change the setting `terminal.integrated.shell.windows` to wherever you 
installed cygwin. You will then have to also edit `nes-starter-kit/.vscode/tasks.json` to change the path used for 
`bash.exe`.

#### Compile and run on pressing ctrl+shift+b

This is already set up for you too! You can change how this works by editing `nes-starter-kit/.vscode/tasks.json`.

#### Suggested VS Code Extensions

These extensions aren't required at all, but add features that could make development more pleasant. 
Check out the store pages, then make the call for yourself!

##### C/C++ (Miscrosoft)

This provides C and C++ syntax highlighting for VS Code. You probably want this one! To note, the application will suggest
installing this automatically when you open a `.c` or `.h.` file for the first time. You don't need to find this one
manually if you don't want to.

##### MADS (Jaromir Obr)

This is a 6502 assembly language support extension. You probably don't need it for most nes-starter-kit projects.
It will provide syntax highlighting for the raw assembly language in neslib, and some of our custom extensions.
You want this if you're planning on modifying or extending the guts of `nes-starter-kit`. If you'd prefer stick with C,
you can avoid this.