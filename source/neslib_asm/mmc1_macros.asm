MMC1_CTRL	=$8000
MMC1_CHR0	=$a000
MMC1_CHR1	=$c000
MMC1_PRG	=$e000


; MMC1 needs a reset stub in every bank that will put us into a known state. This defines it for all banks.
.macro resetstub_in segname
	.segment segname
		.scope
		
			resetstub_entry:
				sei
				ldx #$FF
				txs
				stx MMC1_CTRL  ; Writing $80-$FF anywhere in $8000-$FFFF resets MMC1
				jmp start
				.addr nmi, resetstub_entry, $0000
		.endscope
.endmacro

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
