; All this does is put the raw bytes in the palettes you export from nesst into the rom, then give it
; an address to reference. These are prefixed with an underscore (_) because that's how C knows that they
; are meant to be accessible from our C code. Don't forget to use .export on new items!
.segment "CODE"

_titlePalette:
    .incbin "graphics/palettes/title.pal"

_errorPalette: 
    .incbin "graphics/palettes/error.pal"

_mainBgPalette:
    .incbin "graphics/palettes/main_bg.pal"
    
_mainSpritePalette: 
    .incbin "graphics/palettes/main_sprite.pal"

; The .export command tells our assembler that these are meant to be available to C. So does the name...
; both do that. There's more complex details, but if I'm honest I don't know them myself, so I can't document
; them here.
.export _titlePalette, _errorPalette, _mainBgPalette, _mainSpritePalette