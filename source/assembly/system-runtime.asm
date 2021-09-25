; Startup code for cc65 and Shiru's NES library
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>
.feature c_comments

FT_SFX_STREAMS            = 4            ;number of sound effects played at once, 1..4

.define FT_DPCM_ENABLE  1            ;undefine to exclude all DMC code
.define FT_SFX_ENABLE   1            ;undefine to exclude all sound effects code
.define SOUND_BANK 0

.include "source/assembly/mmc1_macros.asm"
; FIXME: Loss of functionality, the choice was automated here. Need to tell user what to do.
.include "config/ca65_constants.asm"

    .export _exit,__STARTUP__:absolute=1
    .import initlib,push0,popa,popax,_main,zerobss,copydata

; Linker generated symbols
    .import __RAM_START__   ,__RAM_SIZE__
    .import __ROM0_START__  ,__ROM0_SIZE__
    .import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
    .import    __CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
    .import    __RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__
    .import NES_PRG_BANKS,NES_CHR_BANKS
    .include "tools/cc65/asminc/zeropage.inc"

    .export _frameCount



FT_BASE_ADR=$0100            ;page in RAM, should be $xx00

.define FT_THREAD       1    ;undefine if you call sound effects in the same thread as sound update
.define FT_PAL_SUPPORT    0   ;undefine to exclude PAL support
.define FT_NTSC_SUPPORT    1   ;undefine to exclude NTSC support


PPU_CTRL     =$2000
PPU_MASK     =$2001
PPU_STATUS   =$2002
PPU_OAM_ADDR =$2003
PPU_OAM_DATA =$2004
PPU_SCROLL   =$2005
PPU_ADDR     =$2006
PPU_DATA     =$2007
PPU_OAM_DMA  =$4014
DMC_FREQ     =$4010
CTRL_PORT1   =$4016
CTRL_PORT2   =$4017

OAM_BUF      =$0200
PAL_BUF      =$01c0



.segment "ZEROPAGE"

NTSC_MODE:             .res 1
_frameCount: ; By doing this, I make it available to C code with an extern
FRAME_CNT1:            .res 1
FRAME_CNT2:            .res 1
VRAM_UPDATE:           .res 1
NAME_UPD_ADR:          .res 2
NAME_UPD_ENABLE:       .res 1
PAL_UPDATE:            .res 1
PAL_BG_PTR:            .res 2
PAL_SPR_PTR:           .res 2
SCROLL_X:              .res 1
SCROLL_Y:              .res 1
SCROLL_X1:             .res 1
SCROLL_Y1:             .res 1
WRITE1:                .res 1   
WRITE2:                .res 2      ; For extra X/Y split data.
PAD_STATE:             .res 2        ;one byte per controller
PAD_STATEP:            .res 2
PAD_STATET:            .res 2
PPU_CTRL_VAR:          .res 1
PPU_CTRL_VAR1:         .res 1
PPU_MASK_VAR:          .res 1
RAND_SEED:             .res 2
BP_BANK:               .res 1
BP_BANK_TEMP:          .res 1
BANK_WRITE_IP:         .res 1
NMI_BANK_TEMP:         .res 1
FT_TEMP:               .res 3

MUSIC_PLAY:            .res 1
ft_music_addr:         .res 2

BUF_4000:            .res 1
BUF_4001:            .res 1
BUF_4002:            .res 1
BUF_4003:            .res 1
BUF_4004:            .res 1
BUF_4005:            .res 1
BUF_4006:            .res 1
BUF_4007:            .res 1
BUF_4008:            .res 1
BUF_4009:            .res 1
BUF_400A:            .res 1
BUF_400B:            .res 1
BUF_400C:            .res 1
BUF_400D:            .res 1
BUF_400E:            .res 1
BUF_400F:            .res 1

PREV_4003:            .res 1
PREV_4007:            .res 1

TEMP:                 .res 11

PAD_BUF        =TEMP+1

PTR            =TEMP    ;word
LEN            =TEMP+2    ;word
NEXTSPR        =TEMP+4
SCRX           =TEMP+5
SCRY           =TEMP+6
SRC            =TEMP+7    ;word
DST            =TEMP+9    ;word

RLE_LOW        =TEMP
RLE_HIGH       =TEMP+1
RLE_TAG        =TEMP+2
RLE_BYTE       =TEMP+3



.segment "HEADER"

    .byte $4e,$45,$53,$1a
    .byte <NES_PRG_BANKS
    .byte <NES_CHR_BANKS
	.byte %11100011
	.byte $10
    .res 8,0



.segment "STARTUP"

start:
reset:
_exit:

    sei
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL        ;no NMI

initPPU:

    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1
@2:
    bit PPU_STATUS
    bpl @2

clearPalette:

    lda #$3f
    sta PPU_ADDR
    stx PPU_ADDR
    lda #$0f
    ldx #$20
@1:
    sta PPU_DATA
    dex
    bne @1

clearVRAM:

    txa
    ldy #$20
    sty PPU_ADDR
    sta PPU_ADDR
    ldy #$10
@1:
    sta PPU_DATA
    inx
    bne @1
    dey
    bne @1

clearRAM:

    txa
@1:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @1

    lda #4
    jsr _pal_bright
    jsr _pal_clear
    jsr _oam_clear

    jsr    zerobss
    jsr    copydata

    lda #<(__RAM_START__+__RAM_SIZE__)
    sta    sp
    lda    #>(__RAM_START__+__RAM_SIZE__)
    sta    sp+1            ; Set argument stack ptr

    jsr    initlib

    lda #%10000000
    sta <PPU_CTRL_VAR
    sta PPU_CTRL        ;enable NMI
    lda #%00000110
    sta <PPU_MASK_VAR

waitSync3:
    lda <FRAME_CNT1
@1:
    cmp <FRAME_CNT1
    beq @1

detectNTSC:
    ldx #52                ;blargg's code
    ldy #24
@1:
    dex
    bne @1
    dey
    bne @1

    lda PPU_STATUS
    and #$80
    sta <NTSC_MODE

    jsr _ppu_off

    lda #$ff                ;previous pulse period MSB, to not write it when not changed
    sta PREV_4003
    sta PREV_4007
    
    jsr _music_stop

.if(FT_SFX_ENABLE)
    ldx #<sounds_data
    ldy #>sounds_data
    jsr FamiToneSfxInit
.endif

    lda #$fd
    sta <RAND_SEED
    sta <RAND_SEED+1

    lda #0
    sta PPU_SCROLL
    sta PPU_SCROLL            
    sta PPU_OAM_ADDR

    lda #%11111
    jsr mmc1_set_mirroring
    lda #0
    jsr mmc1_set_prg_bank
    jsr mmc1_set_chr_bank_0
    lda #1 
    jsr mmc1_set_chr_bank_1

    lda #0
    ldx #0
    jsr _set_vram_update

    ; This is a little weird - this variable expects to be a non-zero value on startup.
    ; We call this method to set it properly so the average user doesn't see this weirdness.
    ; If you're not the average engine user, hi! Welcome to the code behind the matrix ;)
    jsr _unset_nmi_chr_tile_bank


    jmp _main            ;no parameters

    .include "source/assembly/famitracker_driver/driver.s"
    .include "source/assembly/mapper.asm"
    .include "source/assembly/bank_helpers.asm"
    ; Put neslib into the primary bank
    .segment "CODE"
    .include "source/assembly/neslib.asm"

    .include "graphics/graphics.config.asm"
    .include "graphics/palettes/palettes.config.asm"

.segment "ROM_00"

music_data:

    .incbin "sound/music.bin"
    
music_dummy_data:

    .byte $0D,$00,$0D,$00,$0D,$00,$0D,$00,$00,$10,$0E,$B8,$0B,$0F,$00,$16
    .byte $00,$01,$40,$06,$96,$00,$18,$00,$22,$00,$22,$00,$22,$00,$22,$00
    .byte $22,$00,$00,$3F

.if(FT_SFX_ENABLE)
sounds_data:
    .include "sound/sfx.asm"
.endif

.segment "DMC"

.if(FT_DPCM_ENABLE)
    .incbin "sound/samples.bin"
.endif