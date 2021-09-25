;
; MMC1 Mapper registers, for use with the functions below
; 
.define MMC1_REG_CONTROL $8000
.define MMC1_REG_CHR_0 $A000
.define MMC1_REG_CHR_1 $C000
.define MMC1_REG_PRG $e000

.define MMC1_MIRRORING_ONE_SCREEN_LOW 0
.define MMC1_MIRRORING_ONE_SCREEN_HIGH 1
.define MMC1_MIRRORING_VERTICAL 2
.define MMC1_MIRRORING_HORIZONTAL 3

; Values for which banks to load
.define MMC1_REG_CONTROL_DEFAULT #%11100

.segment "ZEROPAGE"
    ; Used to track whether a register write was interrupted, so we can try again if needed.
    mmc1RegisterWriteDepth: .res 1
.segment "CODE"
    ; Used internally, write to the register in A with the given address
    .macro mmc1_set_register addr
        ; Store the current depth, so we can make sure we weren't interrupted
        ldy mmc1RegisterWriteDepth
        ; Hide a in the x register in case we need it again
        tax
        @redo_register_write:
        ; Increment the depth, so anything else that runs at the same time as this can see it
        inc mmc1RegisterWriteDepth

        ; Write a value to reset the register
        pha
        lda #255
        sta MMC1_REG_CONTROL ; Register doesn't actually matter for reset
        pla
        ; Start the writing!
        .repeat 4
            sta addr
            lsr
        .endrepeat
        sta addr
        ; Bring back a, mainly for the step below
        txa
        ; Alright, did we get interrupted? If so head back
        dec mmc1RegisterWriteDepth
        cpy mmc1RegisterWriteDepth
        bne @redo_register_write
        
    .endmacro

    ; Store mirroring value to mmc1 register
    mmc1_set_mirroring:
    _mmc1_set_mirroring:
        ora MMC1_REG_CONTROL_DEFAULT
        mmc1_set_register MMC1_REG_CONTROL
        rts

    ; Set the first chr bank
    mmc1_set_chr_bank_0:
    _mmc1_set_chr_bank_0:
        mmc1_set_register MMC1_REG_CHR_0
        rts

    ; Set the second chr bank
    mmc1_set_chr_bank_1:
    _mmc1_set_chr_bank_1:
        mmc1_set_register MMC1_REG_CHR_1
        rts

    ; Set the prg bank currently used in the first slot.
    mmc1_set_prg_bank:
    _mmc1_set_prg_bank:
        mmc1_set_register MMC1_REG_PRG
        rts

    initialize_mapper:
        lda MMC1_MIRRORING_VERTICAL
        ; Mirroring writes to mmc1's ctrl register with everything we care about, so do that.
        jsr mmc1_set_mirroring
        ; Set bank 0 and 1 to the first 2 chr banks
        lda #0
        jsr mmc1_set_chr_bank_0
        lda #1
        jsr mmc1_set_chr_bank_1
        rts

    .export _mmc1_set_mirroring, _mmc1_set_chr_bank_0, _mmc1_set_chr_bank_1, _mmc1_set_prg_bank
; Every bank needs a reset method at the start to get the mapper to start in the right state. So, do that.
.segment "ROM_00" 
    jmp reset
.segment "ROM_01" 
    jmp reset
.segment "ROM_02" 
    jmp reset
.segment "ROM_03" 
    jmp reset
.segment "ROM_04" 
    jmp reset
.segment "ROM_05" 
    jmp reset
.segment "ROM_06" 
    jmp reset

; Throw a single jmp to reset in every bank other than the main PRG bank. This accomplishes 2 things:
; 1) Puts something in the bank, so we avoid warnings
; 2) If we somehow end up here by accident, we'll reset correctly.
.repeat (SYS_PRG_BANKS-1), I
	first_byte_reset_in .concat("ROM_", .sprintf("%02X", I))
.endrepeat

.segment "STUB"
	resetstub:
		sei
		ldx #$ff
		txs
		stx $C000
		jmp start
		.addr nmi, resetstub, irq