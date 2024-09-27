;----------------------------------------------
; Fast loader VIC20
; CC65 conversion by Kees van Oss 2024
;----------------------------------------------

.segment "ZEROPAGE"
;	.include "engine-zp.inc"

;----------------------------------------------
; Header
;----------------------------------------------

.org $a800-2

	.word main
main:
	.include "load_defs.inc"
	.include "load.inc"
	.include "load_core.inc"

eind_asm:

.out .sprintf("Gamecode size = %d bytes, free space = %d bytes", (eind_asm - main),(24576-eind_asm + main))
