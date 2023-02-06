;
; Updates the APU registers. x and y are free to use
;

.if 0
; Found this on nesdev bbs by blargg,
; this can replace the volume table but takes a little more CPU
ft_get_volume:

	lda var_ch_VolColumn, x
	lsr a
	lsr a
	lsr a
	sta var_Temp
	lda var_ch_Volume, x
	sta var_Temp2

    lda var_Temp				; 4x4 multiplication
    lsr var_Temp2
    bcs :+
    lsr a
:   lsr var_Temp2
    bcc :+
    adc var_Temp
:   lsr a
    lsr var_Temp2
    bcc :+
    adc var_Temp
:   lsr a
    lsr var_Temp2
    bcc :+
    adc var_Temp
:   lsr a
	beq :+
	rts
:	lda var_Temp
	ora var_ch_Volume, x
	beq :+
	lda #$01					; Round up to 1
:	rts
.endif

ft_update_apu:
	lda var_PlayerFlags
	bne @Play
	lda #$00					; Kill all channels
	sta $4015
	rts
@KillSweepUnit:					; Reset sweep unit to avoid strange problems
	lda #$C0
	sta $4017
	lda #$40
	sta $4017
	rts
@Play:

; ==============================================================================
;  Square 1
; ==============================================================================
	lda var_Channels
	and #$01
	bne :+
	jmp @Square2
:	lda var_ch_Note				; Kill channel if note = off
	beq @KillSquare1

	; Calculate volume
.if 0
	ldx #$00
	jsr ft_get_volume
	beq @KillSquare1
.endif
	; Calculate volume
	lda var_ch_VolColumn + 0		; Kill channel if volume column = 0
	asl a
	beq @KillSquare1
	and #$F0
	sta var_Temp
	lda var_ch_Volume + 0
	beq @KillSquare1
	ora var_Temp
	tax
	lda ft_volume_table, x
    sec
    sbc var_ch_TremoloResult
    bpl :+
    lda #$00
:   bne :+
    lda var_ch_VolColumn + 0
    beq :+
    lda #$01
:

	; Write to registers
	pha
	lda var_ch_DutyCycle
	and #$03
	tax
	pla
	ora ft_duty_table, x		; Add volume
	ora #$30					; And disable length counter and envelope
	;sta $4000
	sta BUF_4000
	; Period table isn't limited to $7FF anymore
	lda var_ch_PeriodCalcHi
	and #$F8
	beq @TimerOverflow1
	lda #$07
	sta var_ch_PeriodCalcHi
	lda #$FF
	sta var_ch_PeriodCalcLo
@TimerOverflow1:

	lda var_ch_Sweep 			; Check if sweep is active
	beq @NoSquare1Sweep
	and #$80
	beq @Square2				; See if sweep is triggered, if then don't touch sound registers until next note

	lda var_ch_Sweep 			; Trigger sweep
	;sta $4001
	sta BUF_4001
	and #$7F
	sta var_ch_Sweep

	jsr @KillSweepUnit

	lda var_ch_PeriodCalcLo
	;sta $4002
	sta BUF_4002
	lda var_ch_PeriodCalcHi
	;sta $4003
	sta BUF_4003
	;lda #$FF
	;sta var_ch_PrevFreqHigh

	jmp @Square2

@KillSquare1:
	lda #$30
	;sta $4000
	sta BUF_4000
	jmp @Square2

@NoSquare1Sweep:				; No Sweep
	lda #$08
	;sta $4001
	sta BUF_4001
	jsr @KillSweepUnit
	lda var_ch_PeriodCalcLo
	;sta $4002
	sta BUF_4002
	lda var_ch_PeriodCalcHi
	;cmp var_ch_PrevFreqHigh
	;beq @SkipHighPartSq1
	;sta $4003
	sta BUF_4003
	;sta var_ch_PrevFreqHigh
@SkipHighPartSq1:
;	jmp @Square2

; ==============================================================================
;  Square 2
; ==============================================================================
@Square2:
	lda var_Channels
	and #$02
	bne :+
	jmp @Triangle
:	lda var_ch_Note + 1
	beq @KillSquare2

	; Calculate volume
.if 0
	ldx #$01
	jsr ft_get_volume
	beq @KillSquare2
.endif

	lda var_ch_VolColumn + 1		; Kill channel if volume column = 0
	asl a
    beq @KillSquare2
	and #$F0
	sta var_Temp
	lda var_ch_Volume + 1
	beq @KillSquare2
	ora var_Temp
	tax
	lda ft_volume_table, x
    sec
    sbc var_ch_TremoloResult + 1
    bpl :+
    lda #$00
:   bne :+
    lda var_ch_VolColumn + 1
    beq :+
    lda #$01
:

	; Write to registers
	pha
	lda var_ch_DutyCycle + 1
	and #$03
	tax
	pla
	ora ft_duty_table, x
	ora #$30
	;sta $4004
	sta BUF_4004
	; Period table isn't limited to $7FF anymore
	lda var_ch_PeriodCalcHi + 1
	and #$F8
	beq @TimerOverflow2
	lda #$07
	sta var_ch_PeriodCalcHi + 1
	lda #$FF
	sta var_ch_PeriodCalcLo + 1
@TimerOverflow2:

	lda var_ch_Sweep + 1		; Check if there should be sweep
	beq @NoSquare2Sweep
	and #$80
	beq @Triangle				; See if sweep is triggered
	lda var_ch_Sweep + 1		; Trigger sweep
	;sta $4005
	sta BUF_4005
	and #$7F
	sta var_ch_Sweep + 1
	
	jsr @KillSweepUnit

	lda var_ch_PeriodCalcLo + 1	; Could this be done by that below? I don't know
	;sta $4006
	sta BUF_4006
	lda var_ch_PeriodCalcHi + 1
	;sta $4007
	sta BUF_4007
	;lda #$FF
	;sta var_ch_PrevFreqHigh + 1

	jmp @Triangle

@KillSquare2:
	lda #$30
	;sta $4004
	sta BUF_4004
	jmp @Triangle

@NoSquare2Sweep:				; No Sweep
	lda #$08
	;sta $4005
	sta BUF_4005
	jsr @KillSweepUnit
	lda var_ch_PeriodCalcLo + 1
	;sta $4006
	sta BUF_4006
	lda var_ch_PeriodCalcHi + 1
	;cmp var_ch_PrevFreqHigh + 1
	;beq @SkipHighPartSq2
	;sta $4007
	sta BUF_4007
	;sta var_ch_PrevFreqHigh + 1
@SkipHighPartSq2:

@Triangle:
	lda var_Channels
	and #$04
	beq @Noise

; ==============================================================================
;  Triangle
; ==============================================================================
	lda var_ch_Volume + 2
	beq @KillTriangle
    lda var_ch_VolColumn + 2
	beq @KillTriangle
	lda var_ch_Note + 2
	beq @KillTriangle
	lda #$81
	;sta $4008
	sta BUF_4008
	; Period table isn't limited to $7FF anymore
	lda var_ch_PeriodCalcHi + 2
	and #$F8
	beq @TimerOverflow3
	lda #$07
	sta var_ch_PeriodCalcHi + 2
	lda #$FF
	sta var_ch_PeriodCalcLo + 2
@TimerOverflow3:	
;	lda #$08
;	sta $4009
	lda var_ch_PeriodCalcLo + 2
	;sta $400A
	sta BUF_400A
	lda var_ch_PeriodCalcHi + 2
	;sta $400B
	sta BUF_400B
	jmp @SkipTriangleKill
@KillTriangle:
	lda #$00
	;sta $4008
	sta BUF_4008
@SkipTriangleKill:

; ==============================================================================
;  Noise
; ==============================================================================
@Noise:
	lda var_Channels
	and #$08
	beq @DPCM

	lda var_ch_Note + 3
	beq @KillNoise

	; Calculate volume
	lda var_ch_VolColumn + 3		; Kill channel if volume column = 0
	asl a
	beq @KillNoise
	and #$F0
	sta var_Temp
	lda var_ch_Volume + 3
	beq @KillNoise
	ora var_Temp
	tax
	lda ft_volume_table, x
    sec
    sbc var_ch_TremoloResult + 3
    bpl :+
    lda #$00
:   bne :+
    lda var_ch_VolColumn + 3
    beq :+
    lda #$01
:

	; Write to registers
	ora #$30
	;sta $400C
	sta BUF_400C
	lda #$00
	;sta $400D
	sta BUF_400D
	lda var_ch_DutyCycle + 3
;	and #$01
	ror a
	ror a
	and #$80
	sta var_Temp
.if 0
.ifdef SCALE_NOISE
    ; Divide noise period by 16
	lda var_ch_PeriodCalcLo + 3
	lsr a
	lsr a
	lsr a
	lsr a
.else
    ; Limit noise period to range 0 - 15
	lda var_ch_PeriodCalcHi + 3
	bne :+
	lda var_ch_PeriodCalcLo + 3
    cmp #$10
    bcc :++
:   lda #$0F
:   eor #$0F
.endif
.else
; No limit
	lda var_ch_PeriodCalcLo + 3
    and #$0F
    eor #$0F
.endif
	ora var_Temp
	;sta $400E
	sta BUF_400E
	lda #$00
	;sta $400F
	sta BUF_400F
	beq @DPCM
@KillNoise:
	lda #$30
	;sta $400C
	sta BUF_400C
@DPCM:

; ==============================================================================
;  DPCM
; ==============================================================================
.ifdef USE_DPCM
	lda var_Channels
	and #$10
	bne :+
	rts                             ; Skip DPCM
	;beq @Return
:
.ifdef USE_N163
    ldx var_AllChannels
    dex
.else
    ldx #DPCM_CHANNEL
.endif
	lda var_ch_DPCM_Retrig			; Retrigger
	beq @SkipRetrigger
	dec var_ch_DPCM_RetrigCntr
	bne @SkipRetrigger
	sta var_ch_DPCM_RetrigCntr
	lda #$01
	sta var_ch_Note, x
@SkipRetrigger:

	lda var_ch_DPCMDAC				; See if delta counter should be updated
	bmi @SkipDAC
	sta $4011
@SkipDAC:
	lda #$80						; store a negative value to mark that it's already updated
	sta var_ch_DPCMDAC

	lda var_ch_Note, x
	beq @KillDPCM
	bmi @SkipDPCM
	lda var_ch_SamplePitch
	and #$40
	sta var_Temp
	lda var_ch_DPCM_EffPitch
	bpl :+
	lda var_ch_SamplePitch
:   ora var_Temp
    sta $4010
	lda #$80
	sta var_ch_DPCM_EffPitch


	; Setup sample bank (if used)
 .ifdef USE_BANKSWITCH
	lda var_ch_SampleBank
	beq :+
	clc
	sta $5FFC		; Always last bank
	adc #$01
	sta $5FFD
	adc #$01
	sta $5FFE
;	adc #$01
;	sta $5FFF
:
.endif

	; Sample position (add sample offset)
	clc
	lda var_ch_SamplePtr
	adc var_ch_DPCM_Offset
	
;extra DPCM offset, samples now could be included not only at $c000

 .import __DMC_START__
	clc
	adc #<((__DMC_START__-$c000)/64)
	sta $4012

	; Sample length (remove sample offset)
	lda var_ch_DPCM_Offset
	asl a
	asl a
	sta var_Temp
	sec
	lda var_ch_SampleLen
	sbc var_Temp
	sta $4013
	lda #$80
	sta var_ch_Note, x
	lda #$0F
	sta $4015
	lda #$1F
	sta $4015
	rts
@SkipDPCM:
    cmp #$FF
    beq @ReleaseDPCM
	rts
@ReleaseDPCM:
; todo
	lda #$0F
	sta $4015
	lda #$80
	sta var_ch_Note, x
	rts
@KillDPCM:
	lda #$0F
	sta $4015
	lda #$80
	sta $4011
	sta var_ch_Note, x
.endif
@Return:
	rts

; Lookup tables

ft_duty_table:
	.byte $00, $40, $80, $C0

; Volume table: (column volume) * (instrument volume)
ft_volume_table:
	.byte 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	.byte 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
 	.byte 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3
 	.byte 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4
 	.byte 0, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5
 	.byte 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6
 	.byte 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7
 	.byte 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8 
 	.byte 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9 
 	.byte 0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10 
 	.byte 0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11 
 	.byte 0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 8, 9, 10, 11, 12 
 	.byte 0, 1, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13 
 	.byte 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
 	.byte 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 
