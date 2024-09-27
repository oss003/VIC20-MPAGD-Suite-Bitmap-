; *******************************************************************************
; *******************************************************************************
; ** Load from serial bus device						      **
; *******************************************************************************
; *******************************************************************************/

;	subroutine

not_disk:
	jmp	fload2		; use normal KERNAL routine

not_found:
	jmp	fe_ntfnd	; return file not found

k_load:
	ldx	sa		; get secondary address
	sty	sa		; set secondary address for close
	jmp	$f56d		; return to KERNAL

zload:
	ldx	fa		; get device number
	cpx	#8		; compare with first disk device
	bcc	not_disk	; not a serial bus disk device

	sta	verck		; set load/verify flag
	lda	#0		; clear .A
	sta	status		; clear I/O status byte
	.ifndef OPT_PRINT_ADDR
	jsr	srching		; print "SEARCHING..."
	.endif
	ldy	#$f0		; OPEN command
	jsr	ser_str		; send secondary address and filename
	ldy	#0		; clear .Y
	lda	#'$'		; disk directory
	cmp	(fnadr),y	; compare with first filename character
	beq	k_load		; load directory, fall back to KERNAL

	lda	fa		; get device number
	jsr	talk		; command serial bus device to TALK
	lda	#$60		; file read secondary address
	jsr	tksa		; send secondary address after TALK

	jsr	acptr		; get byte from file
	sta	eal		; set current address low byte
	lda	status		; get I/O status byte
	bne	not_found	; timed out, do file not found and return
	jsr	acptr		; get byte from file
	sta	eah		; set current address high byte

	ldx	sa		; get secondary address
	cpx	#1		; compare with use file address
	beq	keep_addr	; skip using supplied address

	.ifconst OPT_CART_SA
	cpx	#2		; compare with ROM image
	bne	set_start

	lda	eah		; get second byte
	pha			; save second byte
	lda	eal		; get first byte
	pha			; save first byte
	.endif

set_start:
	lda	memuss		; get start address low byte
	sta	eal		; set current address low byte
	lda	memuss+1	; get start address high byte
	sta	eah		; set current address high byte

	.ifconst OPT_CART_SA
	cpx	#2		; compare with ROM image
	bne	print_addr

	pla			; restore first byte
	jsr	process	; store or check data value
	pla			; restore second byte
	jsr	process	; store or check data value
print_addr:
	.endif

	.ifconst OPT_PRINT_ADDR
	ldx	#memuss		; address location
	.byte	$2c		; make next instruction BIT
	.endif

keep_addr:
	.ifconst OPT_PRINT_ADDR
	ldx	#eal		; address location
	jsr	print_from_addr	; display "FROM $xxxx"
	else
	jsr	ldvrmsg		; display "LOADING" or "VERIFYING"
	.endif
	bit	jdev		; test JiffyDOS flag
	bpl	k_load2	; not JiffyDOS device, fall back to KERNAL

	; change to sector mode

	jsr	untlk		; command serial bus to UNTALK
	lda	fa		; get device number
	jsr	talk		; command device to TALK
	lda	#$61
	jsr	tksa		; send secondary address after TALK
	ldy	#0		; clear .Y
	sei			; disable interrupts

check_stop:
	jsr	$f755		; get keyboard state for column 3
	lsr			; shift row 0 into Cb
	bcc	close_stop	; [STOP] key down, close channel and exit

	lda	via2_pcr	; get VIA 2 PCR
	and	#%11011101	; mask off DATA & CLK out state
	tax			; copy PCR mask to .X
	ora	#%00100000	; set serial DATA out
	sta	pcr_mask	; save PCR value
	stx	via2_pcr	; set serial DATA & CLK out high

	; wait for serial CLK to go high

wait_up:
	lda	via1_pa2	; get VIA 1 DRA
	lsr			; shift serial CLK into Cb
	bcc	wait_up	; loop if CLK low

	and	#%00000001	; mask serial DATA in
	beq	new_sector	; DATA in low, start of next sector

	; wait for serial DATA to go low, 1.3ms timeout

	ldx	#$6d		; {+2} set repeat counter

wait_down:
	bit	via1_pa2	; {+4} test VIA 1 DRA
	beq	eoi		; {+2/3} serial DATA in low, EOI

	dex			; {+2} decrement counter
	bne	wait_down	; {+3/2} loop until timeout

	lda	#%00000010	; set read timeout bit
	.byte	$2c		; make next instruction BIT

eoi:
	lda	#%01000000	; set EOI bit
	jsr	oriost		; OR into I/O status byte
	.ifconst OPT_PRINT_ADDR
	ldx	#eal		; address location
	jsr	print_to_addr	; display "TO $xxxx"
	.endif

untlk:
	jsr	untlk		; command serial bus to UNTALK
	ldy	#$e0		; CLOSE read command
	jsr	ser_close	; close channel
	.ifconst OPT_PRINT_ADDR
	jsr	print_cr
	.endif
	ldx	eal		; get end address low byte
	ldy	eah		; get end address high byte
	clc			; flag ok
	rts

close_stop:
	jsr	untlk		; close read channel
	sec			; flag error
	rts

k_load2:
	lda	#0		; read channel
	sta	sa		; set secondary address for close
	jmp	$f58a		; return to KERNAL

	; start of sector

new_sector:
	lda	#%00000010	; serial DATA in bitmask

	; wait for serial DATA to go high

wait_up2:
	bit	via1_pa2	; {+4} test VIA 1 DRA
	beq	wait_up2	; {+2/3} loop if DATA in low

next:
	jsr	get_byte	; {6} receive using JiffyDOS sector mode

	.ifconst OPT_CART_SA
	jsr	process	; store or check data value
	bne	next		; get next byte, branch always

	; store or check data value

process:
	.endif

	cpy	verck		; compare load/verify flag
	bne	verify		; verify flag, compare with memory

	sta	(eal),y		; load, store data byte in memory

update_ptr:
	inc	eal		; increment low byte of address
	.ifconst OPT_CART_SA
	bne	out		; no overflow, return
	else
	bne	next		; no overflow, get next byte
	.endif
	inc	eah		; increment high byte of address
	.ifconst OPT_CART_SA
out:
	rts
	else
	bne	next		; get next byte, branch always
	.endif

verify:
	cmp	(eal),y		; verify byte
	beq	update_ptr	; byte matches memory, continue

	lda	#%00010000	; set read error bit
	sta	status		; set I/O status byte
	bne	update_ptr	; update address, branch always
