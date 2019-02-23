// Contains functions to help with working with multle PRG/CHR banks

// Maximum level of recursion to allow with banked_call and similar functions.
// Note that each one of these is a byte of ram from zeropage!
#define MAX_BANK_DEPTH 10

// Switch to another bank and call this function.
// Note: Using banked_call to call a second function from within another banked_call is safe. This will break if you nest
// more than 10 calls deep.
void banked_call(unsigned char bankId, void (*method)(void));

// Switch to the given bank, and keep track of the current bank, so that we may jump back to it as needed.
void bank_push(unsigned char bankId);

// Go back to the last bank pushed on using bank_push.
void bank_pop(void);


// ===== nes-c-boilerplate code start
// NOTE: I ripped this from nes-c-boilerplate, and took out a couple functions that are a little risky to use with this setup.

#define MIRROR_LOWER_BANK 0
#define MIRROR_UPPER_BANK 1
#define MIRROR_VERTICAL 2
#define MIRROR_HORIZONTAL 3

// Switch to the given bank. Your prior bank is not saved, so be sure to save it if you need to switch back.
// bank_id: The bank to switch to.
// returns: The current bank.
unsigned char __fastcall__ set_prg_bank(unsigned char bank_id);

// Get the current bank.
// returns: The current bank.
unsigned char __fastcall__ get_prg_bank(void);

// Set the current 1st 4k chr bank to the bank with this id.
void __fastcall__ set_chr_bank_0(unsigned char bank_id);

// Set the current 2nd 4k chr bank to the bank with this id.
void __fastcall__ set_chr_bank_1(unsigned char bank_id);

// Set the current mirroring mode. Your options are MIRROR_LOWER_BANK, MIRROR_UPPER_BANK,
// MIRROR_HORIZONTAL, and MIRROR_VERTICAL.
// TODO: This could use a better home...
void __fastcall__ set_mirroring(unsigned char mirroring);

// ===== nes-c-boilerplate code end

// Set what chr bank to set at the beginning of every nmi. This allows you to wait for a sprite0 hit, then change
// your chr bank as normal. This allows you to use a separate chr bank for a text engine, or potentially even the
// entire status bar.
void __fastcall__ set_nmi_chr_tile_bank(unsigned char bank);

// Unset the chr bank to set at the beginning of every nmi. Undoes what the function above does.
void __fastcall__ unset_nmi_chr_tile_bank(void);


// A few defines to make common tasks simpler.

// The zero page is 255 bytes of space at the beginning of RAM that is faster to access. The space is shared with rest of
// the game engine. Convert your most heavily used variables to zeropage using this method. This can also save rom space.
// (Usage: ZEROPAGE_DF(type, variableName) ; eg ZEROPAGE_DEF(int, myInt))
// NOTE: If you call this in a .c file and expose it in `globals.h`, be sure to mark it with `ZEROPAGE_EXTERN` there.
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define ZEROPAGE_DEF(defa, defb) \
    _Pragma("bssseg (push,\"ZEROPAGE\")") \
    _Pragma("dataseg (push, \"ZEROPAGE\")") \
    defa defb; \
    _Pragma("bssseg (pop)") \
    _Pragma("dataseg (pop)")

#define ZEROPAGE_ARRAY_DEF(defa, defb, defArr) \
    _Pragma("bssseg (push,\"ZEROPAGE\")") \
    _Pragma("dataseg (push, \"ZEROPAGE\")") \
    defa defb[defArr]; \
    _Pragma("bssseg (pop)") \
    _Pragma("dataseg (pop)")

#define SRAM_DEF(defa, defb) \
    _Pragma("bssseg (push,\"SRAM\")") \
    _Pragma("dataseg (push, \"SRAM\")") \
    defa defb; \
    _Pragma("bssseg (pop)") \
    _Pragma("dataseg (pop)")

#define SRAM_ARRAY_DEF(defa, defb, defArr) \
    _Pragma("bssseg (push,\"SRAM\")") \
    _Pragma("dataseg (push, \"SRAM\")") \
    defa defb[defArr]; \
    _Pragma("bssseg (pop)") \
    _Pragma("dataseg (pop)")



// Mark a variable referenced in a header file as being a zeropage symbol.
// Any time you set a variable as a ZEROPAGE_DEF, you will want to also update any header files referencing it
// with this function.
// (Usage; ZEROPAGE_EXTERN(type, variableName); eg ZEROPAGE_EXTERN(int, myInt))
#define ZEROPAGE_EXTERN(defa, defb) extern defa defb; _Pragma("zpsym (\"" STR(defb) "\")")
#define ZEROPAGE_ARRAY_EXTERN(defa, defb, defArr) extern defa defb[defArr]; _Pragma("zpsym (\"" STR(defb) "\")")

// Mark a variable referened in a header file as being SRAM.
// Technically this just creates a regular extern, and you could avoid using this symbol. It is used only for
// consistency with ZEROPAGE variables, to make the source easier to follow.
#define SRAM_EXTERN(defa, defb) extern defa defb;
#define SRAM_ARRAY_EXTERN(defa, defb, defArr) extern defa defb[defArr];

// Set the PRG bank to put the code in the current file into.
#define CODE_BANK(id) _Pragma("rodataseg (push, \"ROM_0" STR(id) "\")") _Pragma("codeseg (push, \"ROM_0" STR(id) "\")")

// Reverse the actions of the CODE_BANK function, if you need to go back to the default bank.
#define CODE_BANK_POP() _Pragma("rodataseg (pop)") _Pragma("codeseg (pop)")