;NES hardware-dependent functions by Shiru (shiru@mail.ru)
;with improvements by VEG
;Feel free to do anything you want with this code, consider it Public Domain

; NOTE: Edits by cppchriscpp: 
; - Added mmc1 bank swapping
; - Made the nmi and music_play methods support swapping to a set SOUND_BANK before reading data.
; - Added second split method with y split support from na_th_an's NESDev code
; - Messed with the original split method to make it trigger a little later.
; - Problematic PAL bugfix removed; only supporting NTSC with this engine.

;modified to work with the FamiTracker music driver

	.export _pal_all,_pal_bg,_pal_spr,_pal_col,_pal_clear
	.export _pal_bright,_pal_spr_bright,_pal_bg_bright
	.export _ppu_off,_ppu_on_all,_ppu_on_bg,_ppu_on_spr,_ppu_mask,_ppu_system
	.export _oam_clear,_oam_size,_oam_spr,_oam_meta_spr,_oam_hide_rest
	.export _ppu_wait_frame,_ppu_wait_nmi
	.export _scroll,_split
	.export _bank_spr,_bank_bg
	.export _vram_read,_vram_write
	.export _music_play,_music_stop,_music_pause
	.export _sfx_play
	.export _pad_poll,_pad_trigger,_pad_state
	.export _rand8,_rand16,_set_rand
	.export _vram_adr,_vram_put,_vram_fill,_vram_inc,_vram_unrle
	.export _set_vram_update,_flush_vram_update
	.export _memcpy,_memfill,_delay

	.export _split_y


;NMI handler

nmi:
	pha
	txa
	pha
	tya
	pha

	lda <PPU_MASK_VAR	;if rendering is disabled, do not access the VRAM at all
	and #%00011000
	bne @doUpdate
	jmp	@skipAll

@doUpdate:

	lda #>OAM_BUF		;update OAM
	sta PPU_OAM_DMA

	lda <PAL_UPDATE		;update palette if needed
	bne @updPal
	jmp @updVRAM

@updPal:

	ldx #0
	stx <PAL_UPDATE

	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR

	ldy PAL_BUF				;background color, remember it in X
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	tax
	
	.repeat 3,I
	ldy PAL_BUF+1+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat

	.repeat 3,J		
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+5+(J*4)+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

	.repeat 4,J		
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+17+(J*4)+I
	lda (PAL_SPR_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

@updVRAM:

	lda <VRAM_UPDATE
	beq @skipUpd
	lda #0
	sta <VRAM_UPDATE
	
	lda <NAME_UPD_ENABLE
	beq @skipUpd

	jsr _flush_vram_update_nmi

@skipUpd:

	lda #0
	sta PPU_ADDR
	sta PPU_ADDR

	lda <SCROLL_X
	sta PPU_SCROLL
	lda <SCROLL_Y
	sta PPU_SCROLL

	lda <PPU_CTRL_VAR
	sta PPU_CTRL

@skipAll:

	lda <PPU_MASK_VAR
	sta PPU_MASK

	inc <FRAME_CNT1
	inc <FRAME_CNT2
	lda <FRAME_CNT2
	cmp #6
	bne @skipNtsc
	lda #0
	sta <FRAME_CNT2

@skipNtsc:
    ; Bank swapping time! Switch to the music bank for sfx, etc...
    lda BP_BANK
    sta NMI_BANK_TEMP
    lda #SOUND_BANK
    jsr _set_prg_bank

	;play music, the code is modified to put data into output buffer instead of APU registers
    

	lda <MUSIC_PLAY
	ror
	bcc :+
	jsr ft_music_play
	jmp :++
:
	lda #$30			;mute channels when music does not play
	sta <BUF_4000
	sta <BUF_4004
	sta <BUF_400C
	lda #$00
	sta <BUF_4008
:

	;process all sound effect streams
	
	.if(FT_SFX_ENABLE)
	
	.if FT_SFX_STREAMS>0
	ldx #FT_SFX_CH0
	jsr _FT2SfxUpdate
	.endif
	.if FT_SFX_STREAMS>1
	ldx #FT_SFX_CH1
	jsr _FT2SfxUpdate
	.endif
	.if FT_SFX_STREAMS>2
	ldx #FT_SFX_CH2
	jsr _FT2SfxUpdate
	.endif
	.if FT_SFX_STREAMS>3
	ldx #FT_SFX_CH3
	jsr _FT2SfxUpdate
	.endif
	
	.endif

	;send data from the output buffer to the APU

	lda <BUF_4000
	sta $4000
	lda <BUF_4001
	sta $4001
	lda <BUF_4002
	sta $4002

	lda <BUF_4003
	cmp <PREV_4003
	beq :+
	sta <PREV_4003
	sta $4003
:

	lda <BUF_4004
	sta $4004
	lda <BUF_4005
	sta $4005
	lda <BUF_4006
	sta $4006

	lda <BUF_4007
	cmp <PREV_4007
	beq :+
	sta <PREV_4007
	sta $4007
:

	lda <BUF_4008
	sta $4008
	lda <BUF_4009
	sta $4009
	lda <BUF_400A
	sta $400A
	lda <BUF_400B
	sta $400B
	lda <BUF_400C
	sta $400C
	lda <BUF_400D
	sta $400D
	lda <BUF_400E
	sta $400E
	lda <BUF_400F
	sta $400F

    lda NMI_BANK_TEMP
    jsr _set_prg_bank

	pla
	tay
	pla
	tax
	pla

irq:

    rti



;famitone sound effects code and structures

FT_VARS=FT_BASE_ADR

	.if(FT_PAL_SUPPORT)
	.if(FT_NTSC_SUPPORT)
FT_PITCH_FIX = (FT_PAL_SUPPORT|FT_NTSC_SUPPORT)			;add PAL/NTSC pitch correction code only when both modes are enabled
	.endif
	.endif


;zero page variables

FT_TEMP_PTR			= FT_TEMP		;word
FT_TEMP_PTR_L		= FT_TEMP_PTR+0
FT_TEMP_PTR_H		= FT_TEMP_PTR+1
FT_TEMP_VAR1		= FT_TEMP+2

;sound effect stream variables, 2 bytes and 15 bytes per stream
;when sound effects are disabled, this memory is not used

FT_PAL_ADJUST	    = FT_VARS+0
FT_SFX_ADR_L		= FT_VARS+1
FT_SFX_ADR_H		= FT_VARS+2
FT_SFX_BASE_ADR		= FT_VARS+3

FT_SFX_STRUCT_SIZE	= 15
FT_SFX_REPEAT		= FT_SFX_BASE_ADR+0
FT_SFX_PTR_L		= FT_SFX_BASE_ADR+1
FT_SFX_PTR_H		= FT_SFX_BASE_ADR+2
FT_SFX_OFF			= FT_SFX_BASE_ADR+3
FT_SFX_BUF			= FT_SFX_BASE_ADR+4	;11 bytes


;aliases for sound effect channels to use in user calls

FT_SFX_CH0			= FT_SFX_STRUCT_SIZE*0
FT_SFX_CH1			= FT_SFX_STRUCT_SIZE*1
FT_SFX_CH2			= FT_SFX_STRUCT_SIZE*2
FT_SFX_CH3			= FT_SFX_STRUCT_SIZE*3



	.if(FT_SFX_ENABLE)

;------------------------------------------------------------------------------
; init sound effects player, set pointer to data
; in: X,Y is address of sound effects data
;------------------------------------------------------------------------------

FamiToneSfxInit:

	stx <TEMP+0
	sty <TEMP+1
	
	ldy #0

; @cppchriscpp change:
; Disable the famitracker PAL features... it has a weird bug where if I disable PAL, this var
; is no longer defined, and ca65 does not like that.	
;	.if(FT_PITCH_FIX)

;	lda FT_PAL_ADJUST		;add 2 to the sound list pointer for PAL
;	bne @ntsc
;	iny
;	iny
@ntsc:

;	.endif
	
	lda (TEMP),y		;read and store pointer to the effects list
	sta FT_SFX_ADR_L
	iny
	lda (TEMP),y
	sta FT_SFX_ADR_H

	ldx #FT_SFX_CH0			;init all the streams

@set_channels:

	jsr _FT2SfxClearChannel
	txa
	clc
	adc #FT_SFX_STRUCT_SIZE
	tax
	cpx #FT_SFX_CH0+FT_SFX_STRUCT_SIZE*FT_SFX_STREAMS
	bne @set_channels

	rts


;internal routine, clears output buffer of a sound effect
;in: A is 0
;    X is offset of sound effect stream

_FT2SfxClearChannel:

	lda #0
	sta FT_SFX_PTR_H,x		;this stops the effect
	sta FT_SFX_REPEAT,x
	sta FT_SFX_OFF,x
	sta FT_SFX_BUF+6,x		;mute triangle
	lda #$30
	sta FT_SFX_BUF+0,x		;mute pulse1
	sta FT_SFX_BUF+3,x		;mute pulse2
	sta FT_SFX_BUF+9,x		;mute noise

	rts


;------------------------------------------------------------------------------
; play sound effect
; in: A is a number of the sound effect 0..127
;     X is offset of sound effect channel, should be FT_SFX_CH0..FT_SFX_CH3
;------------------------------------------------------------------------------

FamiToneSfxPlay:

	asl a					;get offset in the effects list
	tay

	jsr _FT2SfxClearChannel	;stops the effect if it plays

	lda FT_SFX_ADR_L
	sta <TEMP+0
	lda FT_SFX_ADR_H
	sta <TEMP+1

	lda (TEMP),y		;read effect pointer from the table
	sta FT_SFX_PTR_L,x		;store it
	iny
	lda (TEMP),y
	sta FT_SFX_PTR_H,x		;this write enables the effect

	rts


	
;internal routine, update one sound effect stream
;in: X is offset of sound effect stream

_FT2SfxUpdate:

	lda FT_SFX_REPEAT,x		;check if repeat counter is not zero
	beq @no_repeat
	dec FT_SFX_REPEAT,x		;decrement and return
	bne @update_buf			;just mix with output buffer

@no_repeat:
	lda FT_SFX_PTR_H,x		;check if MSB of the pointer is not zero
	bne @sfx_active
	rts						;return otherwise, no active effect

@sfx_active:
	sta <FT_TEMP_PTR_H		;load effect pointer into temp
	lda FT_SFX_PTR_L,x
	sta <FT_TEMP_PTR_L
	ldy FT_SFX_OFF,x
	clc

@read_byte:
	lda (FT_TEMP_PTR),y		;read byte of effect
	bmi @get_data			;if bit 7 is set, it is a register write
	beq @eof
	iny
	sta FT_SFX_REPEAT,x		;if bit 7 is reset, it is number of repeats
	tya
	sta FT_SFX_OFF,x
	jmp @update_buf

@get_data:
	iny
	stx <FT_TEMP_VAR1		;it is a register write
	adc <FT_TEMP_VAR1		;get offset in the effect output buffer
	and #$7f
	tax
	lda (FT_TEMP_PTR),y		;read value
	iny
	sta FT_SFX_BUF,x		;store into output buffer
	ldx <FT_TEMP_VAR1
	jmp @read_byte			;and read next byte

@eof:
	sta FT_SFX_PTR_H,x		;mark channel as inactive

@update_buf:

	lda <BUF_4000			;compare effect output buffer with main output buffer
	and #$0f				;if volume of pulse 1 of effect is higher than that of the
	sta <FT_TEMP_VAR1		;main buffer, overwrite the main buffer value with the new one
	lda FT_SFX_BUF+0,x
	and #$0f
	cmp <FT_TEMP_VAR1
	bcc @no_pulse1
	lda FT_SFX_BUF+0,x
	sta <BUF_4000
	lda FT_SFX_BUF+1,x
	sta <BUF_4002
	lda FT_SFX_BUF+2,x
	sta <BUF_4003
@no_pulse1:

	lda <BUF_4004		;same for pulse 2
	and #$0f
	sta <FT_TEMP_VAR1
	lda FT_SFX_BUF+3,x
	and #$0f
	cmp <FT_TEMP_VAR1
	bcc @no_pulse2
	lda FT_SFX_BUF+3,x
	sta <BUF_4004
	lda FT_SFX_BUF+4,x
	sta <BUF_4006
	lda FT_SFX_BUF+5,x
	sta <BUF_4007
@no_pulse2:

	lda FT_SFX_BUF+6,x		;overwrite triangle of main output buffer if it is active
	beq @no_triangle
	sta <BUF_4008
	lda FT_SFX_BUF+7,x
	sta <BUF_400A
	lda FT_SFX_BUF+8,x
	sta <BUF_400B
@no_triangle:

	lda <BUF_400C			;same as for pulse 1 and 2, but for noise
	and #$0f
	sta <FT_TEMP_VAR1
	lda FT_SFX_BUF+9,x
	and #$0f
	cmp <FT_TEMP_VAR1
	bcc @no_noise
	lda FT_SFX_BUF+9,x
	sta <BUF_400C
	lda FT_SFX_BUF+10,x
	sta <BUF_400E
@no_noise:

	rts

	.endif
	
	
	
;void __fastcall__ pal_all(const char *data);

_pal_all:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$20

pal_copy:

	sta <LEN

	ldy #$00

@0:

	lda (PTR),y
	sta PAL_BUF,x
	inx
	iny
	dec <LEN
	bne @0

	inc <PAL_UPDATE

	rts



;void __fastcall__ pal_bg(const char *data);

_pal_bg:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$10
	bne pal_copy ;bra



;void __fastcall__ pal_spr(const char *data);

_pal_spr:

	sta <PTR
	stx <PTR+1
	ldx #$10
	txa
	bne pal_copy ;bra



;void __fastcall__ pal_col(unsigned char index,unsigned char color);

_pal_col:

	sta <PTR
	jsr popa
	and #$1f
	tax
	lda <PTR
	sta PAL_BUF,x
	inc <PAL_UPDATE
	rts



;void __fastcall__ pal_clear(void);

_pal_clear:

	lda #$0f
	ldx #0

@1:

	sta PAL_BUF,x
	inx
	cpx #$20
	bne @1
	stx <PAL_UPDATE
	rts



;void __fastcall__ pal_spr_bright(unsigned char bright);

_pal_spr_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_SPR_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_SPR_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bg_bright(unsigned char bright);

_pal_bg_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_BG_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_BG_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bright(unsigned char bright);

_pal_bright:

	jsr _pal_spr_bright
	txa
	jmp _pal_bg_bright



;void __fastcall__ ppu_off(void);

_ppu_off:

	lda <PPU_MASK_VAR
	and #%11100111
	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_all(void);

_ppu_on_all:

	lda <PPU_MASK_VAR
	ora #%00011000

ppu_onoff:

	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_bg(void);

_ppu_on_bg:

	lda <PPU_MASK_VAR
	ora #%00001000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_on_spr(void);

_ppu_on_spr:

	lda <PPU_MASK_VAR
	ora #%00010000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_mask(unsigned char mask);

_ppu_mask:

	sta <PPU_MASK_VAR
	rts



;unsigned char __fastcall__ ppu_system(void);

_ppu_system:

	lda <NTSC_MODE
	rts



;void __fastcall__ oam_clear(void);

_oam_clear:

	ldx #0
	lda #$ff
@1:
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts



;void __fastcall__ oam_size(unsigned char size);

_oam_size:

	asl a
	asl a
	asl a
	asl a
	asl a
	and #$20
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$df
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;unsigned char __fastcall__ oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr,unsigned char sprid);

_oam_spr:

	tax

	ldy #0		;four popa calls replacement
	lda (sp),y
	iny
	sta OAM_BUF+2,x
	lda (sp),y
	iny
	sta OAM_BUF+1,x
	lda (sp),y
	iny
	sta OAM_BUF+0,x
	lda (sp),y
	sta OAM_BUF+3,x

	lda <sp
	clc
	adc #4
	sta <sp
	bcc @1
	inc <sp+1

@1:

	txa
	clc
	adc #4
	rts



;unsigned char __fastcall__ oam_meta_spr(unsigned char x,unsigned char y,unsigned char sprid,const unsigned char *data);

_oam_meta_spr:

	sta <PTR
	stx <PTR+1

	ldy #2		;three popa calls replacement, performed in reversed order
	lda (sp),y
	dey
	sta <SCRX
	lda (sp),y
	dey
	sta <SCRY
	lda (sp),y
	tax

@1:

	lda (PTR),y		;x offset
	cmp #$80
	beq @2
	iny
	clc
	adc <SCRX
	sta OAM_BUF+3,x
	lda (PTR),y		;y offset
	iny
	clc
	adc <SCRY
	sta OAM_BUF+0,x
	lda (PTR),y		;tile
	iny
	sta OAM_BUF+1,x
	lda (PTR),y		;attribute
	iny
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @1

@2:

	lda <sp
	adc #2			;carry is always set here, so it adds 3
	sta <sp
	bcc @3
	inc <sp+1

@3:

	txa
	rts



;void __fastcall__ oam_hide_rest(unsigned char sprid);

_oam_hide_rest:

	tax
	lda #240

@1:

	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts



;void __fastcall__ ppu_wait_frame(void);

_ppu_wait_frame:

	lda #1
	sta <VRAM_UPDATE
	lda <FRAME_CNT1

@1:

	cmp <FRAME_CNT1
	beq @1
	lda <NTSC_MODE
	beq @3

@2:

	lda <FRAME_CNT2
	cmp #5
	beq @2

@3:

	rts



;void __fastcall__ ppu_wait_nmi(void);

_ppu_wait_nmi:

	lda #1
	sta <VRAM_UPDATE
	lda <FRAME_CNT1
@1:

	cmp <FRAME_CNT1
	beq @1
	rts



;void __fastcall__ vram_unrle(const unsigned char *data);

_vram_unrle:

	tay
	stx <RLE_HIGH
	lda #0
	sta <RLE_LOW

	lda (RLE_LOW),y
	sta <RLE_TAG
	iny
	bne @1
	inc <RLE_HIGH

@1:

	lda (RLE_LOW),y
	iny
	bne @11
	inc <RLE_HIGH

@11:

	cmp <RLE_TAG
	beq @2
	sta PPU_DATA
	sta <RLE_BYTE
	bne @1

@2:

	lda (RLE_LOW),y
	beq @4
	iny
	bne @21
	inc <RLE_HIGH

@21:

	tax
	lda <RLE_BYTE

@3:

	sta PPU_DATA
	dex
	bne @3
	beq @1

@4:

	rts



;void __fastcall__ scroll(unsigned int x,unsigned int y);

_scroll:

	sta <TEMP

	txa
;	bne @1
	lda <TEMP
	cmp #240
	bcs @1
	sta <SCROLL_Y
	lda #0
	sta <TEMP
	beq @2	;bra

@1:

	sec
	lda <TEMP
	sbc #240
	sta <SCROLL_Y
	lda #2
	sta <TEMP

@2:

	jsr popax
	sta <SCROLL_X
	txa
	and #$01
	ora <TEMP
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$fc
	ora <TEMP
	sta <PPU_CTRL_VAR
	rts



;;void __fastcall__ split(unsigned int x,unsigned int y);

_split:

	jsr popax
	sta <SCROLL_X1
	txa
	and #$01
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$fc
	ora <TEMP
	sta <PPU_CTRL_VAR1

@3:

	bit PPU_STATUS
	bvs @3

@4:

	bit PPU_STATUS
	bvc @4

	; Wait a few cycles to align with the *next* line.
	; @cppchriscpp hack
	ldx #0
	@looper:
		inx
		cpx #44
		bne @looper


	lda <SCROLL_X1
	sta PPU_SCROLL
	lda #0
	sta PPU_SCROLL
	lda <PPU_CTRL_VAR1
	sta PPU_CTRL

	rts

;;void __fastcall__ split_y(unsigned int x,unsigned int y);

; Using na_tha_an's trickery for y scroll from here: https://forums.nesdev.com/viewtopic.php?f=2&t=16435
; Thanks dude!
   ; Extract SCROLL_Y1, SCROLL_X1, WRITE1 from parameters.

_split_y:
   sta <TEMP

   txa
   bne @1
   lda <TEMP
   cmp #240
   bcs @1
   sta <SCROLL_Y1
   lda #0
   sta <TEMP
   beq @2   ;bra

@1:
   sec
   lda <TEMP
   sbc #240
   sta <SCROLL_Y1
   lda #8               ;; Bit 3
   sta <TEMP
@2:

   jsr popax
   sta <SCROLL_X1
   txa
   and #$01
   asl a
   asl a                ;; Bit 2
   ora <TEMP               ;; From Y
   sta <WRITE1            ;; Store!

   ; Calculate WRITE2 = ((Y & $F8) << 2) | (X >> 3)

   lda <SCROLL_Y1
   and #$F8
   asl a
   asl a
   sta <TEMP             ;; TEMP = (Y & $F8) << 2
   lda <SCROLL_X1
   lsr a
   lsr a
   lsr a                ;; A = (X >> 3)
   ora <TEMP             ;; A = (X >> 3) | ((Y & $F8) << 2)
   sta <WRITE2            ;; Store!

   ; Wait for sprite 0 hit

@3:
   bit PPU_STATUS
   bvs @3
@4:
   bit PPU_STATUS
   bvc @4

	; Wait a few cycles to align with the *next* line.
	; @cppchriscpp hack
	ldx #0
	@looper:
		inx
		cpx #44
		bne @looper


   ; Set scroll value
   lda PPU_STATUS
   lda <WRITE1
   sta PPU_ADDR
   lda <SCROLL_Y1
   sta PPU_SCROLL
   lda <SCROLL_X1
   ldx <WRITE2
   sta PPU_SCROLL
   stx PPU_ADDR
   
   rts

;void __fastcall__ bank_spr(unsigned char n);

_bank_spr:

	and #$01
	asl a
	asl a
	asl a
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #%11110111
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;void __fastcall__ bank_bg(unsigned char n);

_bank_bg:

	and #$01
	asl a
	asl a
	asl a
	asl a
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #%11101111
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;void __fastcall__ vram_read(unsigned char *dst,unsigned int size);

_vram_read:

	sta <TEMP
	stx <TEMP+1

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	lda PPU_DATA

	ldy #0

@1:

	lda PPU_DATA
	sta (TEMP+2),y
	inc <TEMP+2
	bne @2
	inc <TEMP+3

@2:

	lda <TEMP
	bne @3
	dec <TEMP+1

@3:

	dec <TEMP
	lda <TEMP
	ora <TEMP+1
	bne @1

	rts



;void __fastcall__ vram_write(unsigned char *src,unsigned int size);

_vram_write:

	sta <TEMP
	stx <TEMP+1

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	ldy #0

@1:

	lda (TEMP+2),y
	sta PPU_DATA
	inc <TEMP+2
	bne @2
	inc <TEMP+3

@2:

	lda <TEMP
	bne @3
	dec <TEMP+1

@3:

	dec <TEMP
	lda <TEMP
	ora <TEMP+1
	bne @1

	rts



;void __fastcall__ music_play(unsigned char song);

_music_play:

	; @cppchriscpp Edit - forcing a swap to the music bank
	; Need to temporarily swap banks to pull this off. 
	tax ; Put our song into x for a moment...
	; Being extra careful and setting BP_BANK to ours in case an nmi fires while we're doing this.
	lda BP_BANK
	pha
	lda SOUND_BANK
	sta BP_BANK
	mmc1_register_write MMC1_PRG
	txa ; bring back the song number!


	ldx #<music_data
	stx <ft_music_addr+0
	ldx #>music_data
	stx <ft_music_addr+1

	ldx <NTSC_MODE
	jsr ft_music_init
	
	lda #1
	sta <MUSIC_PLAY

	; Remember when we stored the old bank into BP_BANK and swapped? Time to roll back.
	pla
	sta BP_BANK
	mmc1_register_write MMC1_PRG

	rts



;void __fastcall__ music_stop(void);

_music_stop:

	ldx #<music_dummy_data
	stx <ft_music_addr+0
	ldx #>music_dummy_data
	stx <ft_music_addr+1
	
	lda #0
	ldx <NTSC_MODE
	jsr ft_music_init
	
	lda #0
	sta <MUSIC_PLAY
	rts



;void __fastcall__ music_pause(unsigned char pause);

_music_pause:

	inc <MUSIC_PLAY
	
	rts



;void __fastcall__ sfx_play(unsigned char sound,unsigned char channel);

_sfx_play:

.if(FT_SFX_ENABLE)
	; TODO: Should I be blocking interrupts while doing weird bank stuff?
	; @cppchriscpp Edit - forcing a swap to the music bank
	; Need to temporarily swap banks to pull this off. 
	tay ; Put our song into y for a moment...
	; Being extra careful and setting BP_BANK to ours in case an nmi fires while we're doing this.
	lda BP_BANK
	pha
	lda #SOUND_BANK
	sta BP_BANK
	mmc1_register_write MMC1_PRG
	tya ; bring back the song number!

	and #$03
	tax
	lda @sfxPriority,x
	tax
	jsr popa
	jsr FamiToneSfxPlay

	; Remember when we stored the old bank into BP_BANK and swapped? Time to roll back.
	pla
	sta BP_BANK
	mmc1_register_write MMC1_PRG

	rts

@sfxPriority:

	.byte FT_SFX_CH0,FT_SFX_CH1,FT_SFX_CH2,FT_SFX_CH3
	
.else
	rts
.endif



;unsigned char __fastcall__ pad_poll(unsigned char pad);

_pad_poll:

	tay
	ldx #0

@padPollPort:

	lda #1
	sta CTRL_PORT1
	lda #0
	sta CTRL_PORT1
	lda #8
	sta <TEMP

@padPollLoop:

	lda CTRL_PORT1,y
	lsr a
	ror <PAD_BUF,x
	dec <TEMP
	bne @padPollLoop

	inx
	cpx #3
	bne @padPollPort

	lda <PAD_BUF
	cmp <PAD_BUF+1
	beq @done
	cmp <PAD_BUF+2
	beq @done
	lda <PAD_BUF+1

@done:

	sta <PAD_STATE,y
	tax
	eor <PAD_STATEP,y
	and <PAD_STATE ,y
	sta <PAD_STATET,y
	txa
	sta <PAD_STATEP,y

	rts



;unsigned char __fastcall__ pad_trigger(unsigned char pad);

_pad_trigger:

	pha
	jsr _pad_poll
	pla
	tax
	lda <PAD_STATET,x
	rts



;unsigned char __fastcall__ pad_state(unsigned char pad);

_pad_state:

	tax
	lda <PAD_STATE,x
	rts



;unsigned char __fastcall__ rand8(void);
;Galois random generator, found somewhere
;out: A random number 0..255

rand1:

	lda <RAND_SEED
	asl a
	bcc @1
	eor #$cf

@1:

	sta <RAND_SEED
	rts

rand2:

	lda <RAND_SEED+1
	asl a
	bcc @1
	eor #$d7

@1:

	sta <RAND_SEED+1
	rts

_rand8:

	jsr rand1
	jsr rand2
	adc <RAND_SEED
	rts



;unsigned int __fastcall__ rand16(void);

_rand16:

	jsr rand1
	tax
	jsr rand2

	rts


;void __fastcall__ set_rand(unsigned char seed);

_set_rand:

	sta <RAND_SEED
	stx <RAND_SEED+1

	rts



;void __fastcall__ set_vram_update(unsigned char *buf);

_set_vram_update:

	sta <NAME_UPD_ADR+0
	stx <NAME_UPD_ADR+1
	ora <NAME_UPD_ADR+1
	sta <NAME_UPD_ENABLE

	rts



;void __fastcall__ flush_vram_update(unsigned char *buf);

_flush_vram_update:

	sta <NAME_UPD_ADR+0
	stx <NAME_UPD_ADR+1

_flush_vram_update_nmi:

	ldy #0

@updName:

	lda (NAME_UPD_ADR),y
	iny
	cmp #$40				;is it a non-sequental write?
	bcs @updNotSeq
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_DATA
	jmp @updName

@updNotSeq:

	tax
	lda <PPU_CTRL_VAR
	cpx #$80				;is it a horizontal or vertical sequence?
	bcc @updHorzSeq
	cpx #$ff				;is it end of the update?
	beq @updDone

@updVertSeq:

	ora #$04
	bne @updNameSeq			;bra

@updHorzSeq:

	and #$fb

@updNameSeq:

	sta PPU_CTRL

	txa
	and #$3f
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	tax

@updNameLoop:

	lda (NAME_UPD_ADR),y
	iny
	sta PPU_DATA
	dex
	bne @updNameLoop

	lda <PPU_CTRL_VAR
	sta PPU_CTRL

	jmp @updName

@updDone:

	rts
	
	
	
;void __fastcall__ vram_adr(unsigned int adr);

_vram_adr:

	stx PPU_ADDR
	sta PPU_ADDR

	rts



;void __fastcall__ vram_put(unsigned char n);

_vram_put:

	sta PPU_DATA

	rts



;void __fastcall__ vram_fill(unsigned char n,unsigned int len);

_vram_fill:

	sta <LEN
	stx <LEN+1
	jsr popa
	ldx <LEN+1
	beq @2
	ldx #0

@1:

	sta PPU_DATA
	dex
	bne @1
	dec <LEN+1
	bne @1

@2:

	ldx <LEN
	beq @4

@3:

	sta PPU_DATA
	dex
	bne @3

@4:

	rts



;void __fastcall__ vram_inc(unsigned char n);

_vram_inc:

	ora #0
	beq @1
	lda #$04

@1:

	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$fb
	ora <TEMP
	sta <PPU_CTRL_VAR
	sta PPU_CTRL

	rts



;void __fastcall__ memcpy(void *dst,void *src,unsigned int len);

_memcpy:

	sta <LEN
	stx <LEN+1
	jsr popax
	sta <SRC
	stx <SRC+1
	jsr popax
	sta <DST
	stx <DST+1

	ldx #0

@1:

	lda <LEN+1
	beq @2
	jsr @3
	dec <LEN+1
	inc <SRC+1
	inc <DST+1
	jmp @1

@2:

	ldx <LEN
	beq @5

@3:

	ldy #0

@4:

	lda (SRC),y
	sta (DST),y
	iny
	dex
	bne @4

@5:

	rts



;void __fastcall__ memfill(void *dst,unsigned char value,unsigned int len);

_memfill:

	sta <LEN
	stx <LEN+1
	jsr popa
	sta <TEMP
	jsr popax
	sta <DST
	stx <DST+1

	ldx #0

@1:

	lda <LEN+1
	beq @2
	jsr @3
	dec <LEN+1
	inc <DST+1
	jmp @1

@2:

	ldx <LEN
	beq @5

@3:

	ldy #0
	lda <TEMP

@4:

	sta (DST),y
	iny
	dex
	bne @4

@5:

	rts



;void __fastcall__ delay(unsigned char frames);

_delay:

	tax

@1:

	jsr _ppu_wait_nmi
	dex
	bne @1

	rts



palBrightTableL:

	.byte <palBrightTable0,<palBrightTable1,<palBrightTable2
	.byte <palBrightTable3,<palBrightTable4,<palBrightTable5
	.byte <palBrightTable6,<palBrightTable7,<palBrightTable8

palBrightTableH:

	.byte >palBrightTable0,>palBrightTable1,>palBrightTable2
	.byte >palBrightTable3,>palBrightTable4,>palBrightTable5
	.byte >palBrightTable6,>palBrightTable7,>palBrightTable8

palBrightTable0:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f	;black
palBrightTable1:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable2:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable3:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable4:
	.byte $00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0f,$0f,$0f	;normal colors
palBrightTable5:
	.byte $10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1a,$1b,$1c,$00,$00,$00
palBrightTable6:
	.byte $10,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$10,$10,$10	;$10 because $20 is the same as $30
palBrightTable7:
	.byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$20,$20,$20
palBrightTable8:
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30	;white
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30

;	.include "famitone2.s"