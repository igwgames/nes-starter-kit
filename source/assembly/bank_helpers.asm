.feature c_comments
.linecont +

.define NO_CHR_BANK 255

.segment "ZEROPAGE"
    nmiChrTileBank: .res 1
    .exportzp nmiChrTileBank

.segment "CODE"

.export _set_prg_bank, _get_prg_bank, _set_chr_bank_0, _set_chr_bank_1
.export _set_nmi_chr_tile_bank, _unset_nmi_chr_tile_bank
.export _set_mirroring

_set_prg_bank:
    ; Store new bank into BP_BANK
    sta BP_BANK

    jsr mmc1_set_prg_bank

    rts

_set_prg_bank_raw:
    ; Set PRG bank without any of the nmi safety or variable modifying from above. 
    ; You probably don't want to call this one!
    jsr mmc1_set_prg_bank
    rts

_get_prg_bank:
    lda BP_BANK
    rts

_set_chr_bank_0:
    jsr mmc1_set_chr_bank_0
    rts

_set_chr_bank_1:
    jsr mmc1_set_chr_bank_1
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
    ; Limit this to mirroring bits, so we can add our bytes safely.
    and #%00000011
    ; Bombs away!
    jsr mmc1_set_mirroring
    rts