;---------------------------------------------------------------------
; AGD LOADER,
;
; - load AGDSWR at $3c00-$7bff
; - switch SWRAM bank 4
; - upload $2200-$7bff -> $8000-$bfff
; - load AGDRAM at $2600
; - switch SWRAM bank 4
; - start game
;---------------------------------------------------------------------

AGDRAMload	= $2200
AGDSWRload	= $3c00
SWRaddress	= $8000
boot_game	= $2200
OSFILE		= $ffdd

.org $2100

; - load AGDSWR at $3c00-$7bff

	ldx #<loadAGDSWR		; Set pointer to loaderblock1
	ldy #>loadAGDSWR
	lda #$ff
	jsr OSFILE

; - switch SWRAM bank 4

	lda #4				; select SWRAM
	sta $f4
	sta $fe30

; - upload $3b00-$7bff -> $8000-$bfff

	ldx #$40			; Nr of blocks
	ldy #0
uploop:
	lda AGDSWRload, y
	sta SWRaddress, y
	iny
	bne uploop
	inc uploop + 2
	inc uploop + 5
	dex
	bne uploop

; - load AGDRAM at $2200

	ldx #<loadAGDRAM		; Set pointer to loaderblock2
	ldy #>loadAGDRAM
	lda #$ff
	jsr OSFILE

; - upload $2200-$7bff -> $8000-$bfff

	ldx #$5a			; Nr of blocks
	ldy #$ff
uploop1:
	lda $7b00, y
	sta $7f00, y
	dey
	cpy #$ff
	bne uploop1
	dec uploop1 + 2
	dec uploop1 + 5
	dex
	bne uploop1

; - switch SWRAM bank 4

	lda #4				; select SWRAM
	sta $f4
	sta $fe30

; - start game

	jmp boot_game

;---------------------------------------------------------------------

loadAGDSWR:	.byte <file1,>file1
		.byte <AGDSWRload,>AGDSWRload,0,0
		.byte <AGDSWRload,>AGDSWRload,0,0
		.byte 0,0,0,0,0,0,0,0
file1:		.byte "AGDSWR",$0d

loadAGDRAM:	.byte <file2,>file2
		.byte <AGDRAMload,>AGDRAMload,0,0
		.byte <AGDRAMload,>AGDRAMload,0,0
		.byte 0,0,0,0,0,0,0,0
file2:		.byte "AGDRAM",$0d
