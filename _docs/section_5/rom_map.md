---
title: ROM Map
permalink: guide/section_5/rom_map.html
---
# ROM Map

This is meant to visualize where all data in the rom is stored. Each bank is `16kb` in size, and can 
be swapped in as needed. The only exception is the PRG section, which is always loaded. It contains
library code that is always accessible, as well as banking code. 

Any banks marked as unused are great targets for your own code. You can also move things from other
banks into them by changing their associated `PRG_BANK_TYPE` define in the header file.


## Kernel (always loaded - $07)
- NESLib
- Main program loop
- prg/chr/etc banking routines
- Famitracker music library

## ROM_00 ($00)
- Music data
- Sound effect data

## ROM_01 ($01)
- Error handler screen
- Level drawing logic
- NPC text engine

## ROM_02 ($02)
- Title screen
- Various menus
- Pause Menu
- Hud logic
- Player sprite/movement logic

## ROM_03 ($03)
- Level data

## ROM_04 ($04)
- Unused

## ROM_05 ($05)
- Unused

## ROM_06 ($06)
- Unused