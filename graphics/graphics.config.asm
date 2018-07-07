;
; CHR RAM data
;
; The files below will be loaded into the rom, so they can be loaded into memory on the fly.
;

; Loads our main ascii and tiles chr data into code bank 5 (currently unused)
.export _main_tiles, _main_sprites, _ascii_tiles
.segment "ROM_05"
    _ascii_tiles:
        .incbin "graphics/ascii.chr"
        _main_tiles: 
        .incbin "graphics/tiles.chr"
	_main_sprites: 
        .incbin "graphics/sprites.chr"
