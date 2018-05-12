; First, set CHR bank and PRG bank. We'll re-define this in our header file. 
; This is what the CODE_BANK macro does in your C code.
; If you're not sure about this, ignore it for now (there's a FAQ entry about this)

.segment "ROM_04"

; Now, load up the various asset files, and give each of them a C-friendly name.

_custom_title_nametable_rle:
    .incbin "graphics/static/title.rle"

_custom_title_palette: 
    .incbin "graphics/static/title.pal"

; The .export command tells our assembler that these are meant to be available to C. So does the name...
; both do that. There's more complex details, but if I'm honest I don't know them myself, so I can't document
; them here.
.export _custom_title_nametable_rle, _custom_title_palette
