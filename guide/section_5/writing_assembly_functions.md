# Writing Functions in Assembly Language

For the most part, C can probably cover anything you need to do in your game, but sometimes you need to use
assembly - most often this is because you need to write something efficiently, and C won't accomplish that.
Whatever the reason, this isn't hard to support!

Note that I'm assuming you know the basics of 6502 assembly language, as well as C. If not, uh, how did 
you get here? :)

## Create a function Definition

First, we'll start with the basics. Create a new header file to put your function into. This will allow us
to call it from C code. Next, figure out what your function is going to look like. To demonstrate, here 
are two functions we will define further down.

**NOTE**: These example functions are not useful; there are features built into neslib for these. The focus is
on how to interact between assembly and C!

```c
unsigned char __fastcall__ get_current_frame(void);
void __fastcall__ set_random_seed(int seed);
```

These are almost normal function definitions, except for the `__fastcall__` annotation. Don't miss this! It
tells the compiler how your function will use the registers. Also, don't use `__fastcall__` with C functions,
unless you know what you are doing. This can confuse the compiler.

## Create assembly code

Next, we'll need to create an assembly file and start filling it with content. I'm not going to spend much time 
on the actual assembly code, but I'll highlight a few key points.

```asm
; Use the .import macro to make sure you have access to variables defined elsewhere.
.import FRAME_COUNTER, RAND_SEED

; The .export macro tells the compiler to make these symbols available elsewhere.
.export _get_current_frame, _set_random_seed

_get_current_frame
    ; Put a variable into a before you call `rts`, and that will be your return value.
    ; For 16 bit values, put the high byte into x, (using ldx) and low byte into a.
    lda FRAME_COUNTER
    rts

_set_random_seed
    ; Due to using __fastcall__, your value is in the a register. 2 byte values (such as integers) have their high byte in x.
    ; If you have multiple parameters, they are loaded from right to left. A/X will have the far right value,
    ; then you can use the `popa` or `popax` to move left through parameters.
    stx >RAND_SEED
    sta <RAND_SEED
    rts
```

Most of what's going on is explained in comments, however I will call out the underscores before function
names. Any functions available to C must be prefixed with an underscore. This underscore will not be part
of how you call the function - just call it like you wrote it in the header file.

## Hooking up the assembly files

Now we have both a header file with function definitions, and the code to go with. But, the code isn't
actually part of your rom! You will need to add this to your `source/neslib_asm/crt0.asm` file. Look for
where we include `source/graphics/palettes.config.asm` and add yours right after. 

## Okay, now how do I use it?

Finally, we have the easy bit. To use it, include your new header file into a C file wherever you need it,
and call the functions like you would any other C function. You're done!

## How did you learn this? Where can I learn more?

The author of neslib wrote some really good documentation on how to do this, as well as how to use neslib
in general. Find that [here](https://shiru.untergrund.net/articles/programming_nes_games_in_c.htm). Look
for the section titled "Writing Functions in Assembly Code".
