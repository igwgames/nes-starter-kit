; CHR RAM data
;
; The files below will be loaded on the fly into chr ram as required.
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

.export _ocean_tiles
.segment "CODE"
	_ocean_tiles: 
		.incbin "graphics/ocean_tiles.chr"
