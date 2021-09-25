.feature c_comments
.linecont +

.define NO_CHR_BANK 255

.segment "ZEROPAGE"
    nmiChrTileBank: .res 1
    regCurrentState: .res 1
    .exportzp nmiChrTileBank, regCurrentState

.segment "CODE"

.export _set_prg_bank, _get_prg_bank, _set_chr_bank_0, _set_chr_bank_1
.export _set_nmi_chr_tile_bank, _unset_nmi_chr_tile_bank
.export _set_mirroring

_set_prg_bank:
    ; Store new bank into BP_BANK
    sta BP_BANK

    ; Set a flag for the nmi method to check, so if it runs in the middle of us writing 
    ; this bank, we can finish what we were doing first.
    lda regCurrentState
    and #%11100000
    clc
    adc BP_BANK
    sta regCurrentState
    sta $C000

    rts

_set_prg_bank_raw:
    ; Set PRG bank without any of the nmi safety or variable modifying from above. 
    ; You probably don't want to call this one!
    sta $C000
    rts

_get_prg_bank:
    lda BP_BANK
    rts

_set_chr_bank_0:
    pha
    lda regCurrentState
    and #%10011111
    sta regCurrentState
    pla
    .repeat 5
        asl
    .endrepeat
    clc
    adc regCurrentState
    sta regCurrentState
    sta $C000
    rts

_set_chr_bank_1:
    ; Not supported!
    rts

; Both of these just set/unset a varible used in `neslib.asm` to trigger this during nmi.
_set_nmi_chr_tile_bank: 
    sta nmiChrTileBank
    rts

_unset_nmi_chr_tile_bank:
    lda #NO_CHR_BANK
    sta nmiChrTileBank
    rts

_set_mirroring:
    ; Unsupported!
    rts