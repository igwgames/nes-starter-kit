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

    ; Set a flag for the nmi method to check, so if it runs in the middle of us writing 
    ; this bank, we can finish what we were doing first.
    ldx #1
    stx BANK_WRITE_IP


    ; Write the new bank to the register (destroys a)
    jsr _set_prg_bank_raw

    ; See if nmi interrupted us mid-update.
    ldx BANK_WRITE_IP
    cpx #2
    bne @no_nmi
        ; Yep, we got interrupted. We need to run the normal nmi method now that we have switched.
        ldx #0
        stx BANK_WRITE_IP
        jsr nmi_update
        rts
    @no_nmi:
    ; We didn't get interrupted, so mark it as complete and move on.
    ldx #0
    stx BANK_WRITE_IP

    rts

_set_prg_bank_raw:
    ; Set PRG bank without any of the nmi safety or variable modifying from above. 
    ; You probably don't want to call this one!
    mmc1_register_write MMC1_PRG
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
    ; Now, set this to have 4k chr banking, and not mess up which prg bank is which.
    ora #%00011100
    ; Bombs away!
    mmc1_register_write MMC1_CTRL
    rts