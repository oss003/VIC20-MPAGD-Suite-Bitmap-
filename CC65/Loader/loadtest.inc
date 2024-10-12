;----------------------------------------------
; BASIC header
;----------------------------------------------

.segment "BASIC"

	.word load
load:   .word @end

; 40 SYS pload
	.byte $28,$00,$9e,$20
	.byte .lobyte(pload/1000 .mod 10) + $30
	.byte .lobyte(pload/100 .mod 10) + $30
	.byte .lobyte(pload/10 .mod 10) + $30
	.byte .lobyte(pload/1 .mod 10) + $30
	.byte $00,$00

@end:   .word 0 


pload:
	lda #0
	ldx #0				; Copy loader code to $0f00
load_loop:
	sta $00,x
	inx
	bne load_loop

; Which device to use (in A the file num 1 is OK - x with 8 is the type of device
;                       usually the first drive, and y set to 1 reads from the header
;                       of the file the location where to save it)
	lda #1
	ldx #8		; Disk device
	ldy #1
	jsr $ffba

; Load the file name (in accumulator the size of the string
;                      containing the name, x low address for FileName,
;                      y high address for FileName)
	lda #9
	ldx #<filename
	ldy #>filename
	jsr $ffbd

; load from disk to ram
	lda #0		; accumulator must be set to 0 to load the image
	ldx #<$ff	; target location low not set (got from header)
	ldy #>$ff	; target location high (got from header)
	jsr $ffd5

endless:
	jmp endless

filename:
	.byte "SLASHER2C"
