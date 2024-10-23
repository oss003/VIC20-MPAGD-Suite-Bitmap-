music_init	= $a900
music_on	= $a903
music_off	= $a906
music_play	= $a909

api:
api4:
	cmp #7
	bne api5
	; Clear new screen ram
	lda #$00
	ldx #$00
clearscreen1000:
	sta $1000,x
	inx
	bne clearscreen1000

	lda Reg0
	and #%10000000
	ora #CentreXMP
	sta Reg0

; move vertical position in the screen

	lda #CentreYMP
	sta Reg1	

	; Set color memory to $9400
	lda Reg2
	and #%01111111
	sta Reg2
	
	; Set screen ram and char ram to $1000
	lda #%11001100
	sta Reg5

	; TODO set correct horizontal and vertical position

	; Set number of columns and rows to 20x24
	lda Reg2
	and #%10000000
	ora #20
	sta Reg2

	lda Reg3
	and #%10000001
	ora #24		; number of rows multipled by two
	sta Reg3

	rts
api5:
	cmp #8
	bne api_end

; max num of columns 22

	lda Reg2
	and #%10000000
	ora #ScrWidth
	sta Reg2

; max num of lines (mult by 2 - 22) for 8x8 chars
; no multiply for 8x16 chars

	lda Reg3
	and #%10000001
	ora #ScrHeight		; number of rows multipled by two
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

	rts	
api_end:
	rts
