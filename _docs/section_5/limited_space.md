---
title: Dealing with limited ROM space
permalink: guide/section_5/limited_space.html
---
# Dealing with limited ROM space

NES cartridges are fairly limited in how much space they can take up - our default game uses 8 16kb banks, for a total
of 128kb. At some point you are almost guaranteed to run into issues with your code being too big to fit into a bank.
The errors looks like the following: 

```bash
ld65.exe: Warning: Memory area overflow in `ROM_03', segment `ROM_03' (3 bytes)
ld65.exe: Error: Start address too low in `ROM_03', segment `STUB_03'
```

If you see this, don't panic! You're going to need to do a little work, but we'll get your game running again. The key
thing to note is the area where this happened - in our example, it is `ROM_03` - this means our 3rd bank is too large.
You can prove this to yourself by elmininating some code from that bank - comment something our, or remove some constant
data and verify that your game compiles.

The other thing you can look at is the output from the build command from create-nes-game. It will show the size of each bank. If it's full, it will show 0 bytes free. (or a very low amount)

```
$ create-nes-game build
... Build details skipped
[create-nes-game] [info] Game built successfully: rom\nes-starter-kit-example.nes 
[create-nes-game] [info] ==================== 
[create-nes-game] [info] Stats for: nes-starter-kit-example.nes
[create-nes-game] [info] Mapper: mmc1 (skrom) | Rom Size: 262160 bytes. (16b header, 131072b prg, 131072b chr)
[create-nes-game] [info] 89984/131072 bytes free
[create-nes-game] [info] Bank Breakdown:
[create-nes-game] [info] Bank 01: 14002/16384 bytes free
[create-nes-game] [info] Bank 02: 11228/16384 bytes free
[create-nes-game] [info] Bank 03: 8901/16384 bytes free
[create-nes-game] [info] Bank 04: 0/16384 bytes free
[create-nes-game] [info] Bank 05: 16362/16384 bytes free
[create-nes-game] [info] Bank 06: 16362/16384 bytes free
[create-nes-game] [info] Bank 07: 16362/16384 bytes free
[create-nes-game] [info] Bank 08: 4517/16384 bytes free
[create-nes-game] [info] [afterStepActions build] [sprite_def2img]  [Sat, 11 Feb 2023 05:54:55 GMT]  Success! Image written to graphics/generated/sprites.png 
```

In the example above, bank 4 has no free space, so we may want to move things around.


Now that you have verified that this is the problem, we can figure out what to do about it.

## Solution 1: Use PRG Banking more effectively

The easiest solution to this problem in most cases is to move the code to another bank. This is especially true if
you run into this issue in the primary (`PRG`) bank. 

Try to find code you can break out into separate methods, then
move them into a new file, put the `CODE_BANK()` macro at the top, then begin calling the methods with the 
`banked_call()` method. 

There is much more detail on how to do this in the
[Putting your code in the right place chapter](../section_3/putting_your_code_in_the_right_place.html).

## Solution 2: Remove unused code from your game, and the engine

If you can't find a way to more logically break up your game, another option may be to remove unused code. The engine
is likely a prime candidate! There is a lot of logic built into the game to support things you may not use. Some
examples include: 

- the static sprite movement methods left/right and up/down in `source/c/sprites/map_sprites.c`
- Various logic to work with 8x8 sprites in `source/c/sprites/map_sprites.c`
- The game over screen in `source/c/menus/game_over.c`
- The pause screen in `source/c/menus/pause.c`
- The credits screen in `source/c/menus/credits.c`
- The title screen in `source/c/menus/title.c`
- The error handling screen in `source/c/menus/error.c` (BUT you'll also have to remove anything using this)

There are probably other things too. If you're somewhat sure you don't use something, the easiest way to test
is to figure out what that code might impact, then remove/comment out the code in question, and test the game to 
see how it fares. If it breaks, you can't remove that code (or at least all of it!) If not, remove it and move on.

It also helps to be using source control (such as git) for this, as if you find weird behavior later, you can go back
to before you made this change, and see how it behaves.

## Solution 3: Try to make existing code more efficient

If you are sure most of your code is useful and can't be broken up, another option is to optimize your code a bit. C is
inherently less quick than assembly language, but that doesn't mean we have to use assembly! (Though you can, if so you
choose to.) There are a number of small things you can do that will make a surprising difference in rom size. 

I usually try to save this option for last, as it can make a really big difference in the size and speed of the rom. The
issue is, you only get this gain once - future attempts will reclaim almost nothing by comparison. If you're really good
at following best practices for the NES you may also not get much back, but we all make mistakes. It's especially easy
when you're new!

As for what to do, check out the [Performance Tweaks Chapter](../section_4/performance.html). Everything in that chapter
can reduce the size of your program code at the same time as it makes the code faster. 

A few things that can have a pronounced impact: 
- Working in multiples of 8 and 16 where possible
- Using char instead of int wherever possible
- Preferring the preincement operator (`++i`) over the postincrement operator (`i++`)

Everything in that chapter can make a difference - your best bet is likely to pick a rule, start applying it and see
how it helps. Repeat this as needed with the other rules.

## Solution 4: Make the game bigger

Okay, either you've exhausted all options, or you're expecting your game to be larger than the confines of the default
game. What else can you do? It turns out you have the option of adding another 8 16kb banks. The next chapter will
walk you through how to do just that. 