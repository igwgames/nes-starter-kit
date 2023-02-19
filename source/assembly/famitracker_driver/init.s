;
; ft_sound_init
;
; Initializes the player and song number
; a = song number
; x = ntsc/pal
;
ft_music_init:
	asl a
	jsr ft_load_song
	; Kill APU registers
	; lda #$00
	; ldx #$0B
; @LoadRegs:
	; sta $4000, x
	; dex
	; bne @LoadRegs
	; ldx #$06
; @LoadRegs2:
	; sta $400D, x
	; dex
	; bne @LoadRegs2
	
	lda #0
	tax
:
	sta BUF_4000,x
	inx
	cpx #$10
	bne :-

	lda #$30		; noise is special
	;sta $400C	
	sta BUF_400C
	lda #$0F
	sta $4015		; APU control
	lda #$08
	;sta $4001
	sta BUF_4001
	;sta $4005
	sta BUF_4005
	lda #$C0
	sta $4017
	lda #$40
	sta $4017		; Disable frame IRQs	

	lda #$FF		; Enable all channels
	sta var_Channels

	sta var_ch_DPCM_EffPitch
	sta var_ch_DPCMDAC

	; Reset some variables for the wave channels
	lda #$00
	tax
:	sta var_ch_NoteCut, x
	sta var_ch_Effect, x
	sta var_ch_EffParam, x
	sta var_ch_PortaToLo, x
	sta var_ch_PortaToHi, x
	sta var_ch_TimerPeriodLo, x
	sta var_ch_TimerPeriodHi, x
	inx
	cpx #WAVE_CHANS
	bne :-

	lda var_SongFlags
	and #$02
	beq :++
	lda #48
	ldx #$00
:	sta var_ch_VibratoPos, x
    inx
	cpx #WAVE_CHANS
	bne :-
    lda #$00
:

	; DPCM
	sta var_ch_NoteCut + (CHANNELS - 1)

.ifdef USE_DPCM
.ifdef USE_N163
    ldx var_AllChannels
    dex
.else
    ldx #DPCM_CHANNEL
.endif
    lda #$80
    sta var_ch_Note, x
.endif

.ifdef USE_VRC6
    lda #$00
    sta $9003
.endif

.ifdef USE_MMC5
	lda #$03
	sta $5015		; Enable channels
.endif

.ifdef USE_N163
    jsr ft_init_n163
.endif

.ifdef USE_VRC7
    jsr ft_init_vrc7
.endif

.ifdef USE_FDS
    jsr ft_init_fds
.endif

	rts

;
; Prepare the player for a song
;
; NSF music data header:
;
; - Song list, 2 bytes
; - Instrument list, 2 bytes
; - DPCM instrument list, 2 bytes
; - DPCM sample list, 2 bytes
; - Flags, 1 byte
; - Pointer to wave tables, 2 bytes, if FDS is enabled
; - NTSC speed divider
; - PAL speed divider
;
ft_load_song:
	pha
	; Get the header
	lda ft_music_addr
	sta var_Temp_Pointer
	lda ft_music_addr + 1
	sta var_Temp_Pointer + 1

	; Read the header and store in RAM
	ldy #$00
@LoadAddresses:
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp_Pointer), y
	adc ft_music_addr
	sta var_Song_list, y
	iny
	lda (var_Temp_Pointer), y			; Song list offset, high addr
	adc ft_music_addr + 1
	sta var_Song_list, y
.else
	lda (var_Temp_Pointer), y
	sta var_Song_list, y
	iny
	lda (var_Temp_Pointer), y			; Song list offset, high addr
	sta var_Song_list, y
.endif
	iny
	cpy #$08							; 4 items
	bne @LoadAddresses

	lda (var_Temp_Pointer), y			; Flags, 1 byte
	sta var_SongFlags
	iny

.ifdef USE_FDS
	; Load FDS wave table pointer
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp_Pointer), y
	adc ft_music_addr
	sta var_Wavetables
	iny
	lda (var_Temp_Pointer), y
	adc ft_music_addr + 1
	sta var_Wavetables + 1
.else
	lda (var_Temp_Pointer), y
	sta var_Wavetables
	iny
	lda (var_Temp_Pointer), y
	sta var_Wavetables + 1
.endif
	iny
.endif

	cpx #$01							; PAL / NTSC flag
	beq @LoadPAL
.ifdef NTSC_PERIOD_TABLE
	; Load NTSC speed divider and frequency table
	lda (var_Temp_Pointer), y
	iny
	sta var_Tempo_Dec
	lda (var_Temp_Pointer), y
	iny
	sta var_Tempo_Dec + 1
	lda #<ft_periods_ntsc
	sta var_Note_Table
	lda #>ft_periods_ntsc
	sta var_Note_Table + 1
.ifdef USE_N163
    iny
    iny
.endif
.endif
	jmp @LoadDone
@LoadPAL:
.ifdef PAL_PERIOD_TABLE
	; Load PAL speed divider and frequency table
	iny
	iny
	lda (var_Temp_Pointer), y
	iny
	sta var_Tempo_Dec
	lda (var_Temp_Pointer), y
	iny
	sta var_Tempo_Dec + 1
	lda #<ft_periods_pal
	sta var_Note_Table
	lda #>ft_periods_pal
	sta var_Note_Table + 1
.endif
 @LoadDone:
 .ifdef USE_N163
    ; N163 channel count
	lda (var_Temp_Pointer), y
	iny
    sta var_NamcoChannels
    clc
    adc #$04             ; TODO fix this, should not be hardcoded
    sta var_EffChannels
    adc #$01
    sta var_AllChannels
    
    ldx var_NamcoChannels
    dex
    txa
    asl a
    asl a
    asl a
    asl a
    sta var_NamcoChannelsReg

 .endif
	pla
	tay
	; Load the song
	jsr ft_load_track

	; Clear variables to zero
	; Important!
	ldx #$01
	stx var_PlayerFlags				; Player flags, bit 0 = playing
	dex
@ClearChannels2:					; This clears the first four channels
	lda #$7F
	sta var_ch_VolColumn, x
	lda #$80
	sta var_ch_FinePitch, x
	lda #$00
	;
	;lda #$00
	sta var_ch_VibratoSpeed, x
	sta var_ch_TremoloSpeed, x
	sta var_ch_Effect, x
	sta var_ch_VolSlide, x
	sta var_ch_NoteDelay, x
	sta var_ch_ArpeggioCycle, x
	;
	sta var_ch_Note, x
	inx

.ifdef USE_N163
    cpx var_EffChannels
.else
	cpx #(CHANNELS - 1)
.endif
    bne @ClearChannels2

	ldx #$FF
	;stx var_ch_PrevFreqHigh			; Set prev freq to FF for Sq1 & 2
	;stx var_ch_PrevFreqHigh + 1

.ifdef USE_DPCM
    lda #$00
    sta var_ch_DPCM_Offset
.endif
.ifdef USE_MMC5
	stx var_ch_PrevFreqHighMMC5
	stx var_ch_PrevFreqHighMMC5 + 1
.endif
.ifdef USE_VRC7
	stx var_ch_vrc7_CustomPatch
.endif

	inx								; Jump to the first frame
	stx var_Current_Frame
	jsr ft_load_frame

	jsr ft_calculate_speed
	;jsr ft_restore_speed

	lda #$00
	sta var_Tempo_Accum
	sta var_Tempo_Accum + 1

	rts

;
; Load the track number in A
;
; Track headers:
;
;	- Frame list address, 2 bytes
;	- Number of frames, 1 byte
;	- Pattern length, 1 byte
;	- Speed, 1 byte
;	- Tempo, 1 byte
;
ft_load_track:
	; Load track header address
	lda var_Song_list
	sta var_Temp16
	lda var_Song_list + 1
	sta var_Temp16 + 1

	; Get the real address, song number * 2 will be in Y here
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

	; Read header
	lda #$00
	tax
	tay
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp_Pointer), y			; Frame offset, low addr
	adc ft_music_addr
	sta var_Frame_List
	iny
	lda (var_Temp_Pointer), y			; Frame offset, high addr
	adc ft_music_addr + 1
	sta var_Frame_List + 1
.else
	lda (var_Temp_Pointer), y			; Frame offset, low addr
	sta var_Frame_List
	iny
	lda (var_Temp_Pointer), y			; Frame offset, high addr
	sta var_Frame_List + 1
.endif
	iny
@ReadLoop:
	lda (var_Temp_Pointer), y			; Frame count
	sta var_Frame_Count, x
	iny
	inx
	cpx #$06
	bne @ReadLoop

	rts

;
; Load the frame in A for all channels
;
ft_load_frame:
.ifdef USE_BANKSWITCH
	pha						; Frame bank
	lda var_InitialBank
	beq :+
;	sta $5FFA
	sta $5FFB
:	pla
.endif

	; Get the entry in the frame list
	asl A					; Multiply by two
	clc						; And add the frame list addr to get
	adc var_Frame_List		; the pattern list addr
	sta var_Temp16
	lda #$00
	tay
	tax
	adc var_Frame_List + 1
	sta var_Temp16 + 1
	; Get the entry in the pattern list
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
	; Iterate through the channels, x = channel
	ldy #$00							; Y = address
	stx var_Pattern_Pos
@LoadPatternAddr:
.ifdef RELOCATE_MUSIC
	clc
	lda (var_Temp_Pointer), y			; Load the pattern address for the channel
	adc ft_music_addr
	sta var_ch_PatternAddrLo, x
	iny
	lda (var_Temp_Pointer), y			; Pattern address, high byte
	adc ft_music_addr + 1
	sta var_ch_PatternAddrHi, x
	iny
.else
	lda (var_Temp_Pointer), y			; Load the pattern address for the channel
	sta var_ch_PatternAddrLo, x
	iny
	lda (var_Temp_Pointer), y			; Pattern address, high byte
	sta var_ch_PatternAddrHi, x
	iny
.endif
	lda #$00
	sta var_ch_NoteDelay, x
	sta var_ch_Delay, x
;	sta var_ch_LoopCounter, x
	lda #$FF
	sta var_ch_DefaultDelay, x
	inx
.ifdef USE_N163
    cpx var_AllChannels
.else
	cpx #CHANNELS
.endif
	bne @LoadPatternAddr
; Bankswitch values
.ifdef USE_BANKSWITCH
	lda var_SongFlags					; Check bankswitch flag
	and #$01
	beq @SkipBankValues					; Skip if no bankswitch info is stored
	ldx #$00
@LoadBankValues:
	lda (var_Temp_Pointer), y			; Pattern bank number
	sta var_ch_Bank, x
	iny
	inx
.ifdef USE_N163
    cpx var_AllChannels
.else
	cpx #CHANNELS
.endif
	bne @LoadBankValues
@SkipBankValues:
.endif

	lda #$00
	sta var_Jump
	sta var_Skip

.ifdef ENABLE_ROW_SKIP
	lda var_SkipTo
	bne ft_SkipToRow
	rts
;
; Skip to a certain row, this is NOT recommended in songs when CPU time is critical!!
;
ft_SkipToRow:
	sta var_Pattern_Pos
	ldx #$00							; x = channel
@ChannelLoop:
    lda var_Pattern_Pos
	sta var_Temp2						; Restore row count
	lda #$00
	sta var_ch_NoteDelay, x

@RowLoop:
	ldy #$00
	lda var_ch_PatternAddrLo, x
	sta var_Temp_Pattern
	lda var_ch_PatternAddrHi, x
	sta var_Temp_Pattern + 1

@ReadNote:
	lda var_ch_NoteDelay, x				; First check if in the middle of a row delay
	beq @NoRowDelay
	dec var_ch_NoteDelay, x				; Decrease one
	jmp @RowIsDone

@NoRowDelay:
	; Read a row
	lda (var_Temp_Pattern), y
	bmi @Effect

	lda var_ch_DefaultDelay, x
	cmp #$FF
	bne @LoadDefaultDelay
	iny
	lda (var_Temp_Pattern), y
	iny

	sta var_ch_NoteDelay, x
	jmp @RowIsDone
@LoadDefaultDelay:
	iny
	sta var_ch_NoteDelay, x				; Store default delay
@RowIsDone:
	; Save the new address
	clc
	tya
	adc var_Temp_Pattern
	sta var_ch_PatternAddrLo, x
	lda #$00
	adc var_Temp_Pattern + 1
	sta var_ch_PatternAddrHi, x

	dec var_Temp2						; Next row
	bne @RowLoop

	inx									; Next channel
.ifdef USE_N163
    cpx var_AllChannels
.else
    cpx #CHANNELS
.endif
	bne @ChannelLoop
	lda #$00
	sta var_SkipTo
	rts

@Effect:
    cmp #$80
    beq @LoadInstCmd
	cmp #$82
	beq @EffectDuration
	cmp #$84
	beq @EffectNoDuration
	pha
	cmp #$8E
	beq @OneByteCommand
	cmp #$92
	beq @OneByteCommand
	cmp #$A2
	beq @OneByteCommand
	and #$F0
	cmp #$F0							; See if volume
	beq @OneByteCommand
	cmp #$E0							; See if a quick instrument command
	beq @LoadInst
	iny									; Command takes two bytes
@OneByteCommand:						; Command takes one byte
	iny
	pla
	jmp @ReadNote						; A new command or note is immediately following
@EffectDuration:
	iny
	lda (var_Temp_Pattern), y
	iny
	sta var_ch_DefaultDelay, x
	jmp @ReadNote
@EffectNoDuration:
	iny
	lda #$FF
	sta var_ch_DefaultDelay, x
	jmp @ReadNote
@LoadInstCmd:    ; mult-byte
    iny
	lda (var_Temp_Pattern), y
	iny
	jsr ft_load_instrument
	jmp @ReadNote
@LoadInst:       ; single byte
	iny
	pla
	and #$0F
	asl a
	jsr ft_load_instrument
	jmp @ReadNote

.else   ; ENABLE_ROW_SKIP
	rts
.endif  ; ENABLE_ROW_SKIP
