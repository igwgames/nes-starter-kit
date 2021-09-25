; Defines the number of PRG banks in the game. We have to store this separately, as the values
; in the .cfg file are only available at link-time, and cannot be used in code generation.
; Sorry for the duplication! 
.define SYS_PRG_BANKS 32