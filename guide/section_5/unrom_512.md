# Unrom 512 

This project also supports using Unrom 512/mapper 30. (AKA the NESMaker mapper) This has some weird caveats that
don't show up for MMC1, but also enables some new features. If you're on this page, you probably already know why
you want it. Note that in this branch, some features of the library won't work, or will need manual changes 
(on your part!)

The code for this is available in the `mapper_30` git branch. Note that this builds off of the `chr_ram` branch.

## Pros/cons

This section tries to note the pros and cons of unrom 512 vs MMC1 with chr ram. Please note that the caveats for 
chr ram also apply here, so be sure to read that chapter!

### Some advantages of unrom 512

* 32k of chr ram (not rom, ram!) split across 4 banks
* 512kb worth of prg space, across 32 16kb banks
* 4 screen mirroring (optional, not enabled in the branch)

### Some disadvantages
* No PRG ram to work with (including save ram) - any use of this will not work on cartridges!
    * If you want to save data to the cartridge, you can use the self-flashing feature, but it is not straightforward. 
    * There is no example of self-flashing in this repository (though PRs are welcome!) 
    * Some explanation and example (asm) code is available [on the nesdev wiki](https://wiki.nesdev.org/w/index.php/UNROM_512)
* Mirroring isn't switchable - you have to specify horizontal or vertical in the rom header

# What do I have to know to start using it?

Not much - I would strongly recommend reading the chr ram chapter first, since that is a requirement here. Otherwise, 
the branch should get you up and running!

# What extra helpers are available?

## Changing chr ram banks

You can switch what chr ram bank you are looking at with the `set_chr_bank_0(id)` method, included in 
`library/bank_helpers.h`. `set_chr_bank_1` is disabled, since you have to switch both banks at once.

This chapter is admittedly a bit short - if there is more you'd like to see, create an issue or send a 
PR! The main goal was to create a working example.