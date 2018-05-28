; CHR ROM data
;
; The files below will be loaded into the game's chr banks directly, and available for your
; use.
;

.segment "CHR_00"

    ; We just put the ascii tiles into both sprites and tiles. If you want to get more clever you could do something else.
    .incbin "graphics/ascii.chr"
.segment "CHR_01"
    .incbin "graphics/tiles.chr"

; Note: You can put your own separate chr files here to use them... we only use 3 in the demo. This is to avoid warnings,
; and make the rom a predictable size. Note that if you do this you'll have to tweak the engine to support it! There's
; hopefully a guide on how to do this in the repo.
.segment "CHR_02"
    .incbin "graphics/sprites.chr"
.segment "CHR_03"
    .incbin "graphics/tiles_2.chr"
.segment "CHR_04"
    .incbin "graphics/tiles.chr"
.segment "CHR_05"
    .incbin "graphics/tiles.chr"
.segment "CHR_06"
    .incbin "graphics/tiles.chr"
.segment "CHR_07"
    .incbin "graphics/tiles.chr"
.segment "CHR_08"
    .incbin "graphics/tiles.chr"
.segment "CHR_09"
    .incbin "graphics/tiles.chr"
.segment "CHR_0A"
    .incbin "graphics/tiles.chr"
.segment "CHR_0B"
    .incbin "graphics/tiles.chr"
.segment "CHR_0C"
    .incbin "graphics/tiles.chr"
.segment "CHR_0D"
    .incbin "graphics/tiles.chr"
.segment "CHR_0E"
    .incbin "graphics/tiles.chr"
.segment "CHR_0F"
    .incbin "graphics/tiles.chr"
.segment "CHR_10"
    .incbin "graphics/tiles.chr"
.segment "CHR_11"
    .incbin "graphics/tiles.chr"
.segment "CHR_12"
    .incbin "graphics/tiles.chr"
.segment "CHR_13"
    .incbin "graphics/tiles.chr"
.segment "CHR_14"
    .incbin "graphics/tiles.chr"
.segment "CHR_15"
    .incbin "graphics/tiles.chr"
.segment "CHR_16"
    .incbin "graphics/tiles.chr"
.segment "CHR_17"
    .incbin "graphics/tiles.chr"
.segment "CHR_18"
    .incbin "graphics/tiles.chr"
.segment "CHR_19"
    .incbin "graphics/tiles.chr"
.segment "CHR_1A"
    .incbin "graphics/tiles.chr"
.segment "CHR_1B"
    .incbin "graphics/tiles.chr"
.segment "CHR_1C"
    .incbin "graphics/tiles.chr"
.segment "CHR_1D"
    .incbin "graphics/tiles.chr"
.segment "CHR_1E"
    .incbin "graphics/tiles.chr"
.segment "CHR_1F"
    .incbin "graphics/tiles.chr"
