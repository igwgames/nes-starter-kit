
// Mirroring values to be used with the mirroring button below.
#define MMC1_MIRRORING_ONE_SCREEN_LOW 0
#define MMC1_MIRRORING_ONE_SCREEN_HIGH 1
#define MMC1_MIRRORING_VERTICAL 2
#define MMC1_MIRRORING_HORIZONTAL 3

// Store mirroring value to mmc1 register
extern void __fastcall__ mmc1_set_mirroring(unsigned char mirroring);
// Set the first chr bank
extern void __fastcall__ mmc1_set_chr_bank_0(unsigned char bank);
// Set the second chr bank
extern void __fastcall__ mmc1_set_chr_bank_1(unsigned char bank);
// Set the prg bank currently used in the first slot.
extern void __fastcall__ mmc1_set_prg_bank(unsigned char bank);

