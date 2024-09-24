;=================================================
; Testprogram loader.prg
;=================================================

; KERNEL/BASIC ROM Routines
LOAD        = $FFD5
SETLFS      = $FFBA
SETNAM      = $FFBD

; Constants

	ScreenAddr	= $0200		; VIDEO RAM
	Reg0		= $9000		; Horizontal centering
	Reg1		= $9001		; Vertical centering
	Reg2		= $9002		; Set # of columns
	Reg3		= $9003		; Set # of rows
	Reg5		= $9005		; Start of character memory
	RegF		= $900f		; Screen and border color register
	ColorAttr	= $9600		; Color attributes

	ScrWidth	= 22	; NTSC settings
	ScrHeight	= 22
	CentreX		= 4
	CentreY		= 27

;----------------------------------------------
; BASIC header
;----------------------------------------------

.segment "BASIC"

	.word load
load:   .word @end
	.word 2
	.byte $9e
	.byte .lobyte(main/1000 .mod 10) + $30
	.byte .lobyte(main/100 .mod 10) + $30
	.byte .lobyte(main/10 .mod 10) + $30
	.byte .lobyte(main/1 .mod 10) + $30
	.byte 0 
@end:   .word 0 

main:

;----------------------------------------------
; Initialise screen + hardware
;----------------------------------------------

init:

; Disable interrupts

	lda #$7f
	sta $911e
	sta $912d
	sta $912e

; Set audio volume to max
	lda #$0f
	sta $900e

; Init screen for NTSC
; this sets the background and border color of the screen
; in this case border black and background black

	lda #%00011000			
	sta RegF

; max num of columns 24

	lda Reg2
	and #%10000000
	ora #ScrWidth
	sta Reg2

; max num of lines (mult by 2 - 24) for 8x8 chars
; no multiply for 8x16 chars

	lda Reg3
	and #%10000001
	ora #ScrWidth		; number of rows multipled by two
	sta Reg3

; horizontal and vertical position must be set for 
; non standard screen resolution (standard 22x23)
; same games even allow the user to change them
; these values are for PAL for NTSC should be different
; move horizontal position in the screen

	lda Reg0
	and #%10000000
	ora #CentreX
	sta Reg0

; move vertical position in the screen

	lda #CentreY
	sta Reg1

; Set screem memory to $200
; bit 7 on $9002 has to be set

	lda Reg2
	ora #%10000000
	sta Reg2

; Set character data pointer to CHAR RAM at $1000
; bit 7 on $9005 has always to be set 
; (based on compute - mapping the vic book)
; color ram goes to 9600

	lda Reg5
	and #%00001100
	ora #%10001100
	sta Reg5

; This set the char size to 8x16

	lda Reg3
	ora #%00000001
	sta Reg3

; Init screencharacters
; Fill screen with characters from left to right, up to down

	ldx #0
	ldy #0
scrinit:
	txa
	sta ScreenAddr,x
	tya
	sta ColorAttr,x
	inx
	cpx#242
	bne scrinit

;----------------------------------------------
; Loading picture data + colours
;----------------------------------------------
; SETLS:
;	A = $08		; Logical file number
;	X = $08		; Device number
;	Y = $01		; Secondary address
;				$00 Load using specifed address
;				$01 Load using original address from file
; LOAD:
;	A = $00		; Load = 0, Verify = 1
;	X = <$1000	; Low byte of start address
; 	Y = >$1000	; High byte of start address
;----------------------------------------------

; Load picture data at $1000

	lda #8
	ldx #8
	ldy #1
	jsr SETLFS		; Set above parameters

	lda titlecol		; Length of file name
	ldx #<titlecol+1	; Low byte of file name location
	ldy #>titlecol+1	; High byte of file name location
	jsr SETNAM		; Set the name
	jsr LOAD		; Perform load
	bcs error		; Jump to error handler if error

; Load colour data at $9600

	lda #8
	ldx #8
	ldy #1
	jsr SETLFS		; Set above parameters

	lda titledata		; Length of file name
	ldx #<titledata+1	; Low byte of file name location
	ldy #>titledata+1	; High byte of file name location
	jsr SETNAM		; Set the name
	jsr LOAD		; Perform load
	bcs error		; Jump to error handler if error

; Handle error in A

error:
	rts

;----------------------------------------------
; Data
;----------------------------------------------

titledata:
	.byte 9,"TITLEDATA"
titlecol:
	.byte 8,"TITLECOL"
