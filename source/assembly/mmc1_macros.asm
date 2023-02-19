MMC1_CTRL    =$8000
MMC1_CHR0    =$a000
MMC1_CHR1    =$c000
MMC1_PRG    =$e000


; Throw a single jmp to reset in every bank other than the main PRG bank. This accomplishes 2 things:
; 1) Puts something in the bank, so we avoid warnings
; 2) If we somehow end up here by accident, we'll reset correctly.
.macro first_byte_reset_in segname
    .segment segname
        jmp start
.endmacro

; Quick-n-dirty macro to write to an mmc1 register, which goes one bit at a time, 5 bits wide.
.macro mmc1_register_write addr
    .repeat 4
        sta addr
        lsr
    .endrepeat
    sta addr
.endmacro
