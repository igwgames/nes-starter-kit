; Update the instrument for channel X
;
; I might consider storing the sequence address variables in ZP??
;
ft_run_instrument:
.ifdef USE_VRC7
    lda ft_channel_type, x
    cmp #CHAN_VRC7
    bne :+
    rts
:
.endif
	; Volume
	;
	lda var_ch_SeqVolume + SFX_WAVE_CHANS, x	; High part of address = 0 mean sequence is disabled
	beq @SkipVolumeUpdate
	sta var_Temp_Pointer + 1
	lda var_ch_SeqVolume, x					; Store the sequence address in a zp variable
	sta var_Temp_Pointer
	lda var_ch_SequencePtr1, x				; Sequence item index
	cmp #$FF
	beq @SkipVolumeUpdate					; Skip if end is reached
	jsr ft_run_sequence						; Run an item in the sequence
	sta var_ch_SequencePtr1, x				; Store new index
	lda var_sequence_result					; Take care of the result
	sta var_ch_Volume, x
@SkipVolumeUpdate:

	; Arpeggio
	;
	lda var_ch_SeqArpeggio + SFX_WAVE_CHANS, x
	beq @SkipArpeggioUpdate
	sta var_Temp_Pointer + 1
	lda var_ch_SeqArpeggio, x
	sta var_Temp_Pointer
	lda var_ch_SequencePtr2, x
	cmp #$FF
	beq @RestoreArpeggio;@SkipArpeggioUpdate
	jsr ft_run_sequence
	sta var_ch_SequencePtr2, x
	lda var_ch_Note, x					; No arp if no note
	beq @SkipArpeggioUpdate

	ldy #$03
	lda (var_Temp_Pointer), y
	beq @Absolute
	cmp #$01
	beq	@Fixed
@Relative:
    ; Relative
    clc
	lda var_ch_Note, x
	adc var_sequence_result
	cmp #$01
	bcc :+
	cmp #$5F
	bcc :++
	lda #$5F
	bne :++
:	lda #$01
:	sta var_ch_Note, x
	jmp @ArpDone
@Fixed:
	; Fixed
	lda var_sequence_result
	clc
	adc #$01
	jmp @ArpDone
@Absolute:
	; Absolute
 	clc
	lda var_ch_Note, x
	adc var_sequence_result
	beq :+
	bpl :++
:	lda #$01
:	cmp #$60
	bcc :+
	lda #$60
:
@ArpDone:
	jsr ft_translate_freq_only
	lda #$01
	sta var_ch_ArpFixed, x
	jmp @SkipArpeggioUpdate

@RestoreArpeggio:
	ldy #$03
	lda (var_Temp_Pointer), y
	beq @SkipArpeggioUpdate
	lda var_ch_ArpFixed, x
	beq @SkipArpeggioUpdate
	lda var_ch_Note, x			   ; No arp if no note
	jsr ft_translate_freq_only
	lda #$00
	sta var_ch_ArpFixed, x

@SkipArpeggioUpdate:

	; Pitch bend
	;
	lda var_ch_SeqPitch + SFX_WAVE_CHANS, x
	beq @SkipPitchUpdate
	sta var_Temp_Pointer + 1
	lda var_ch_SeqPitch, x
	sta var_Temp_Pointer
	lda var_ch_SequencePtr3, x
	cmp #$FF
	beq @SkipPitchUpdate
	jsr ft_run_sequence
	sta var_ch_SequencePtr3, x
	
	; Check this
	clc
	lda var_sequence_result
	adc var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodLo, x
	lda var_sequence_result
	bpl @NoNegativePitch 
	lda #$FF
	bmi @LoadLowPitch
@NoNegativePitch:
	lda #$00
@LoadLowPitch:
	adc var_ch_TimerPeriodHi, x
	sta var_ch_TimerPeriodHi, x
	jsr ft_limit_freq
	; ^^^^^^^^^^

	; Save pitch
@SkipPitchUpdate:
	; HiPitch bend
	;
	lda var_ch_SeqHiPitch + SFX_WAVE_CHANS, x
	beq @SkipHiPitchUpdate
	sta var_Temp_Pointer + 1
	lda var_ch_SeqHiPitch, x
	sta var_Temp_Pointer
	lda var_ch_SequencePtr4, x
	cmp #$FF
	beq @SkipHiPitchUpdate
	jsr ft_run_sequence
	sta var_ch_SequencePtr4, x

	; Check this
	lda var_sequence_result
	sta var_Temp16
	rol a
	bcc @AddHiPitch
	lda #$FF
	sta var_Temp16 + 1
	jmp @StoreHiPitch
@AddHiPitch:
	lda #$00
	sta var_Temp16 + 1
@StoreHiPitch:
	ldy #$04
:	clc
	rol var_Temp16 						; multiply by 2
	rol var_Temp16 + 1
	dey
	bne :-
	
	clc	
	lda var_Temp16
	adc var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodLo, x
	lda var_Temp16 + 1
	adc var_ch_TimerPeriodHi, x
	sta var_ch_TimerPeriodHi, x
	jsr ft_limit_freq
	; ^^^^^^^^^^

@SkipHiPitchUpdate:
	; Duty cycle/noise mode
	;
	lda var_ch_SeqDutyCycle + SFX_WAVE_CHANS, x
	beq @SkipDutyUpdate
	sta var_Temp_Pointer + 1
	lda var_ch_SeqDutyCycle, x
	sta var_Temp_Pointer
	lda var_ch_SequencePtr5, x
	cmp #$FF
	beq @SkipDutyUpdate
	jsr ft_run_sequence
	sta var_ch_SequencePtr5, x
	lda var_sequence_result
	pha
	lda var_ch_DutyCycle, x
	and #$F0
	sta var_ch_DutyCycle, x
	pla
	ora var_ch_DutyCycle, x
	sta var_ch_DutyCycle, x
.ifdef USE_N163
	lda ft_channel_type, x
	cmp #CHAN_N163
	bne :+
	jsr ft_n163_load_wave2
:
.endif
	; Save pitch
@SkipDutyUpdate:
	rts


;
; Process a sequence, next position is returned in A
;
; In: A = Sequence index
; Out: A = New sequence index
;
ft_run_sequence:
	clc
	adc #$04						; Offset is 4 items
	tay
	lda (var_Temp_Pointer), y
	sta var_sequence_result
	dey
	dey
	dey ; (remove)
	tya
	ldy #$00						; Check if halt point
	cmp (var_Temp_Pointer), y
	beq @HaltSequence
	ldy #$02						; Check release point
	cmp (var_Temp_Pointer), y
	beq @ReleasePoint
	rts
@HaltSequence:						; Stop the sequence
	iny
	lda (var_Temp_Pointer), y		; Check loop point
	cmp #$FF
	bne @LoopSequence
;	lda #$FF						; Disable sequence by loading $FF into length
	rts
@LoopSequence:						; Just return A
    pha
	lda var_ch_State, x
	bne :+
	pla
	rts								; Return new index
:	ldy #$02						; Check release point
	lda (var_Temp_Pointer), y
	bne :+
	pla								; Release point not found, loop
 	rts
:	pla								; Release point found, don't loop
	lda #$FF
	rts
@ReleasePoint:						; Release point has been reached
	sta	var_Temp					; Save index
	lda var_ch_State, x
    bne @Releasing
	dey
	lda (var_Temp_Pointer), y		; Check loop point
	cmp #$FF
	bne @LoopSequence
	lda var_Temp
	sec								; Step back one step
	sbc #$01
	rts
@Releasing:                         ; Run release sequence
    lda var_Temp
    rts

.macro release sequence, sequence_pointer
	lda sequence + SFX_WAVE_CHANS, x
	beq :+;+
	sta var_Temp_Pointer + 1
	lda sequence, x
	sta var_Temp_Pointer
	ldy #$02
	lda (var_Temp_Pointer), y
	beq :+							; Release not available
	sec
	sbc #$01
    sta sequence_pointer, x
:
.endmacro

; Called on note release instruction
;
ft_instrument_release:
    tya
    pha

	release var_ch_SeqVolume, var_ch_SequencePtr1
	release var_ch_SeqArpeggio, var_ch_SequencePtr2
	release var_ch_SeqPitch, var_ch_SequencePtr3
	release var_ch_SeqHiPitch, var_ch_SequencePtr4
	release var_ch_SeqDutyCycle, var_ch_SequencePtr5

	pla
	tay
	rts

; Reset instrument sequences
;
ft_reset_instrument:

.ifdef USE_FDS
	cpx #FDS_CHANNEL
	bne :+
	lda var_ch_ModDelay
	sta var_ch_ModDelayTick
;	lda #$00
;	sta $4085
;	lda #$80
;	sta $4087
;	rts
:
.endif

	lda #$00
	sta var_ch_SequencePtr1, x
	sta var_ch_SequencePtr2, x
	sta var_ch_SequencePtr3, x
	sta var_ch_SequencePtr4, x
	sta var_ch_SequencePtr5, x
	rts

; Macros


; Macro used to load instrument envelopes
.macro load_inst seq_addr, seq_ptr

	ror var_Temp3
	bcc	:++
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp_Pointer), y
	adc ft_music_addr
	sta var_Temp16
	iny
	lda (var_Temp_Pointer), y
	adc ft_music_addr + 1
	sta var_Temp16 + 1
	iny
.else
	lda (var_Temp_Pointer), y
	sta var_Temp16
	iny
	lda (var_Temp_Pointer), y
	sta var_Temp16 + 1
	iny
.endif

	lda var_Temp16
	cmp seq_addr, x
	bne :+
	lda var_Temp16 + 1
	cmp seq_addr + SFX_WAVE_CHANS, x
	bne :+

	; Both equal, do not touch anything
	jmp :+++

:	lda var_Temp16
	sta seq_addr, x
	lda var_Temp16 + 1
	sta seq_addr + SFX_WAVE_CHANS, x
	lda #$00
	sta seq_ptr, x
	jmp :++		; branch always

:	lda #$00
	sta seq_addr, x
	sta seq_addr + SFX_WAVE_CHANS, x
:

.endmacro

;
; Load instrument (y = saved in var_Temp)
;
; A = instrument number
;
ft_load_instrument:

	; Instrument_pointer_list + a => instrument_address
	; instrument_address + ft_music_addr => instrument_data

	sta var_Temp3	; used by VRC7

	; Get the instrument data pointer
	sty var_Temp
	ldy #$00
	clc
	adc var_Instrument_list
	sta var_Temp16
	tya
	adc var_Instrument_list + 1
	sta var_Temp16 + 1

	; Get the instrument
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp16), y
	adc ft_music_addr
	sta var_Temp_Pointer
	iny
	lda (var_Temp16), y
	adc ft_music_addr + 1
	sta var_Temp_Pointer + 1
.else
	lda (var_Temp16), y
	sta var_Temp_Pointer
	iny
	lda (var_Temp16), y
	sta var_Temp_Pointer + 1
.endif

    ; Jump to the instrument setup routine
;    ldy var_Temp
    lda ft_channel_type, x
	tay
	lda ft_load_inst_pointers, y
	sta var_Temp16
	iny
	lda ft_load_inst_pointers, y
	sta var_Temp16 + 1
	ldy #$00
	jmp (var_Temp16)

ft_load_inst_pointers:
    .word ft_load_instrument_2a03	; 2A03
    .word ft_load_instrument_2a03	; VRC6
    .word ft_load_instrument_vrc7	; VRC7
    .word ft_load_instrument_fds	; FDS
    .word ft_load_instrument_2a03	; MMC5
    .word ft_load_instrument_n163	; N163

; Load 2A03 instrument
ft_load_instrument_2a03:
	; Read instrument data, var_Temp_Pointer points to instrument data
	lda (var_Temp_Pointer), y		; sequence switch
	sta var_Temp3
	iny

    load_inst var_ch_SeqVolume, var_ch_SequencePtr1
    load_inst var_ch_SeqArpeggio, var_ch_SequencePtr2
    load_inst var_ch_SeqPitch, var_ch_SequencePtr3
    load_inst var_ch_SeqHiPitch, var_ch_SequencePtr4
    load_inst var_ch_SeqDutyCycle, var_ch_SequencePtr5

	ldy var_Temp
	rts

; Load FDS instrument
ft_load_instrument_fds:
.ifdef USE_FDS
	; Read FDS instrument
	ldy #$00
	lda (var_Temp_Pointer), y	; Load wave index
	iny
	pha

	; Load modulation table
	jsr ft_reset_modtable
:
	lda (var_Temp_Pointer), y
	pha
	and #$07
	sta $4088
	pla
	lsr a
	lsr a
	lsr a
	sta $4088
	iny
	cpy #$11
	bne :-

	lda (var_Temp_Pointer), y	; Modulation delay
	iny
	sta var_ch_ModDelay
	lda (var_Temp_Pointer), y	; Modulation depth
	iny
	sta var_ch_ModDepth
	lda (var_Temp_Pointer), y	; Modulation freq low
	iny
	sta var_ch_ModRate
	lda (var_Temp_Pointer), y	; Modulation freq high
	sta var_ch_ModRate + 1

	pla							; Get wave index
	jsr ft_load_fds_wave

	; Finish by loading sequences
    ldy #$15
	jmp ft_load_instrument_2a03
.endif

; Load VRC7 instrument
ft_load_instrument_vrc7:
.ifdef USE_VRC7
	; Read VRC7 instrument
	ldy #$00
	lda (var_Temp_Pointer), y		            ; Load patch number
	sta var_ch_vrc7_Patch - VRC7_CHANNEL, x		; vrc7 channel offset
	sta var_ch_vrc7_DefPatch - VRC7_CHANNEL, x
	bne :+							            ; Skip custom settings if patch > 0

	; Store path to custom patch settings
	clc
	lda var_Temp_Pointer
	adc #$01
	sta var_ch_vrc7_CustomLo - VRC7_CHANNEL, x
	lda var_Temp_Pointer + 1
	adc #$00
	sta var_ch_vrc7_CustomHi - VRC7_CHANNEL, x

:	ldy var_Temp
	rts
.endif

; Load N163 instrument
ft_load_instrument_n163:
.ifdef USE_N163
    ldy #$00
    lda (var_Temp_Pointer), y
    sta var_ch_WaveLen - N163_OFFSET, x
    iny
    lda (var_Temp_Pointer), y
    sta var_ch_WavePos - N163_OFFSET, x
    iny
.ifdef RELOCATE_MUSIC
    clc
    lda (var_Temp_Pointer), y
	adc ft_music_addr
    sta var_ch_WavePtrLo - N163_OFFSET, x
    iny
    lda (var_Temp_Pointer), y
	adc ft_music_addr + 1
    sta var_ch_WavePtrHi - N163_OFFSET, x
    iny
.else
    lda (var_Temp_Pointer), y
    sta var_ch_WavePtrLo - N163_OFFSET, x
    iny
    lda (var_Temp_Pointer), y
    sta var_ch_WavePtrHi - N163_OFFSET, x
    iny
.endif
    lda var_NamcoInstrument, x
    cmp var_Temp3
    beq :+
    lda #$00             ; reset wave
    sta var_ch_DutyCycle, x
    lda var_Temp3
    ; Load N163 wave
;    jsr ft_n163_load_wave
:   sta var_NamcoInstrument, x
    jsr ft_load_instrument_2a03
    jsr ft_n163_load_wave2
    ldy var_Temp
    rts
.endif

; Make sure the period doesn't exceed max or min
ft_limit_freq:

    ; Jump to the instrument setup routine
    lda ft_channel_type, x
	tay
	lda ft_limit_pointers, y
	sta var_Temp16
	iny
	lda ft_limit_pointers, y
	sta var_Temp16 + 1
	ldy #$00
	jmp (var_Temp16)


ft_limit_pointers:
    .word ft_limit_period_2a03	     ; 2A03
    .word ft_limit_period_vrc6	     ; VRC6
    .word ft_limit_period_vrc7	     ; VRC7
    .word ft_limit_period_vrc6	     ; FDS
    .word ft_limit_period_2a03	     ; MMC5
    .word ft_limit_period_n163	     ; N163

; N163: no limits
ft_limit_period_n163:
ft_limit_period_vrc7:
    rts

; 2A03: period is between 0 to $7FF
ft_limit_period_2a03:
    lda var_ch_TimerPeriodHi, x
    bmi @LimitMin
    cmp #$08
    bcc @NoLimit
	lda #$07
	sta var_ch_TimerPeriodHi, x
	lda #$FF
	sta var_ch_TimerPeriodLo, x
@NoLimit:
    rts
@LimitMin:
	lda #$00
	sta var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodHi, x
	rts

; VRC6: period is between 0 to $FFF
ft_limit_period_vrc6:
    lda var_ch_TimerPeriodHi, x
    bmi @LimitMin
    cmp #$10
    bcc @NoLimit
	lda #$0F
	sta var_ch_TimerPeriodHi, x
	lda #$FF
	sta var_ch_TimerPeriodLo, x
@NoLimit:
    rts
@LimitMin:
	lda #$00
	sta var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodHi, x
	rts

.if 0

	lda var_ch_TimerPeriodHi, x
	bmi @LimitMin						; period < 0
.ifdef USE_VRC6
	cpx #VRC6_CHANNELS
	bcc :+
	cmp #$10							; period > $FFF
	bcc @NoLimit
	lda #$0F
	sta var_ch_TimerPeriodHi, x
	lda #$FF
	sta var_ch_TimerPeriodLo, x
	rts
:
.endif
.ifdef USE_FDS
	cpx #FDS_CHANNEL
	bne :+
	cmp #$11							; period > $1000?
	bcc @NoLimit
	lda #$10
	sta var_ch_TimerPeriodHi, x
	lda #$FF
	sta var_ch_TimerPeriodLo, x
	rts
:
.endif
	cmp #$08							; period > $7FF
	bcc @NoLimit
	lda #$07
	sta var_ch_TimerPeriodHi, x
	lda #$FF
	sta var_ch_TimerPeriodLo, x
@NoLimit:
	rts
@LimitMin:
	lda #$00
	sta var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodHi, x
	rts

.endif