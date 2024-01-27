# Building Your Game

Once you have finished setting up your tools, building the game is very straightforward. There are two ways 
to do it- through the command line and through your IDE. We will describe both approaches.

## Using Visual Studio Code

1. Press the `ctrl + shift + b` keys at the same time. 

... that's it! This will run the three commands described below, which will result in your game being 
built and opened in your NES emulator.

Read the section below to understand how it works, or how to compile it without this IDE.

## Using the Command Line

1. Open the cygwin terminal using the start menu. Note that this is different from the command prompt (aka 
   cmd) in Windows.

2. Navigate to your game's base directory. (The folder you created with `create-nes-game`, or renamed from `nes-starter-kit`)

3. Type the following command to build your game: `create-nes-game build`. This should not produce any errors. 
    
    If there are errors, look back at the first chapter and make sure all tools are installed. If you are 
    still having trouble, open a github issue with [nes-starter-kit](https://gh.nes.science/nes-starter-kit),
    or otherwise reach out for help!

4. Use the command `create-nes-game run` to start the rom with your preferred emulator.

5. The command `create-nes-game clean` can be used to remove old copies of files that have been renamed or deleted. 
   You shouldn't really have to do this most of the time, but if things are acting strange, it may be useful.

The Visual Studio Code project is set up to always run both commands. Running `make clean` is manual,
but it shouldn't be necessary most of the time.