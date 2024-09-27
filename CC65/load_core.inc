; *******************************************************************************
; * Get byte from serial bus using JiffyDOS protocol                            *
; *                                                                             *
; * Common routine used for both byte and sector mode, returns data in .A.      *
; *******************************************************************************/

;	subroutine

get_byte:
	lda	pcr_mask	; {+3} get value with serial DATA out low
	sta	via2_pcr	; {+4:19.6us} set VIA 2 PCR

	lda	#%00000001	; {+2} serial CLK in bitmask
	bit	via1_pa2	; {+4} test VIA 1 DRA
	beq	end_sector	; {+2/3} serial CLK in low, end of sector

	stx	via2_pcr	; {+4:11.7us} set serial DATA out high

get_byte2:
	lda	via1_pa2	; {11.7us:+4} get VIA 1 DRA with b1&0
	ror			; {+2} shift b0 into b7 ..
	ror			; {+2} .. and b1 into Cb
	and	#%10000000	; {+2} mask b0, b1 in Cb unchanged

	.ifconst SYS_PAL
	nop			; {+2} delay
	.endif

	ora	via1_pa2	; {9.8us:+4} OR VIA 1 DRA with b3&2

	rol			; {+2} shift b3-0 ..
	rol			; {+2} .. to correct position
	sta	lo_nib		; {+3} store lower nibble

	lda	via1_pa2	; {10.8us:+4} get VIA 1 DRA with b5&4
	ror			; {+2} shift b4 to b7 ..
	ror			; {+2} .. and b5 into Cb
	and	#%10000000	; {+2} mask b4, b5 in Cb unchanged

	ora	via1_pa2	; {9.8us:+4} OR VIA 1 DRA with b7&6

	; end of timing sensitive region

	ror			; shift b7-4 ..
	ror
	ror			; .. to correct position
	and	#$f0		; mask top four bits
	sta	hi_nib		; save high nibble
	lda	lo_nib		; get low nibble
	and	#$0f		; mask bottom four bits
	ora	hi_nib		; OR with top nibble
	rts

end_sector:
	pla			; dump calling address low byte
	pla			; dump calling address high byte
	jmp	check_stop	; return to load routine

; *******************************************************************************
; * Close serial channel                                                        *
; *                                                                             *
; * Command secondary address in .Y to close (must include bus command          *
; * e.g. $e0).                                                                  *
; *******************************************************************************/

;	subroutine

ser_close:
	lda	fa		; get device number
	jsr	listen		; command a serial bus device to LISTEN
	tya			; copy secondary address to .A
	jsr	second		; send secondary address after LISTEN
	jmp	unlsn		; command serial bus to UNLISTEN

; *******************************************************************************
; * Command serial bus devices to UNLISTEN                                      *
; *******************************************************************************/

unlsn:
	lda	#$3f		; set the UNLISTEN command
	jsr	cmd_send	; send command byte on serial bus
	jmp	$ef09		; ATN high, delay, CLK high then DATA high

