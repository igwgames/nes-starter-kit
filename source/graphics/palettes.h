// These reference raw palettes that we load in palettes.asm, and export.
// Each one references a palette file exported from nesst. Need a new one?
// Add an entry here, then add an entry to palettes.asm. (Don't forget to update the .export line!)
extern const unsigned char titlePalette[16];
extern const unsigned char errorPalette[16];
extern const unsigned char mainBgPalette[16];
extern const unsigned char mainSpritePalette[16];