.feature c_comments
.linecont +

.export _set_prg_bank, _get_prg_bank, _set_chr_bank_0, _set_chr_bank_1
.export _set_mirroring

_set_prg_bank:
    ; Grab old bank and store it temporarily.
    ldx BP_BANK
    ; Store new bank into BP_BANK
    sta BP_BANK

    ; Write it to the reg (destroys a)
    mmc1_register_write MMC1_PRG
    txa ; Old bank's back!

    rts

_get_prg_bank:
    lda BP_BANK
    rts

_set_chr_bank_0:
    mmc1_register_write MMC1_CHR0
    rts

_set_chr_bank_1:
    mmc1_register_write MMC1_CHR1
    rts

_set_mirroring:
    ; Limit this to mirroring bits, so we can add our bytes safely.
    and #%00000011
    ; Now, set this to have 4k chr banking, and not mess up which prg bank is which.
    ora #%00011100
    ; Bombs away!
    mmc1_register_write MMC1_CTRL
    rts