;----------------------------------------------------------------------
; VIC20 Platform Specific functions
;----------------------------------------------------------------------

;============================================================
; PLATFORM VARIABLES
;============================================================
;
; Memory map:
;
;	$0000-$01ff	 0.5kB	CPU WS
;	$0200-$03ff	 0.5kB	VIDEO RAM
;	$0400-$0fff	 3,0kB	RAM
;	$1000-$1fff	 4,0kB	CHAR RAM
;	$2000-$7fff	24,0kB	RAM
;	$8000-$8fff	 4,0kB	CHAR ROM
;	$9000-$93ff	 1,0kB	VIC VIA's
;	$9400-$97ff	 1,0kB	COLOUR RAM
;	$9800-$9fff	 2,0kB	I/O
;	$a000-$bfff	 8,0kB	RAM
;	$c000-$dfff	 8,0kB	BASIC ROM
;	$e000-$ffff	 8,0kB	KERNAL ROM
;
; Keyboard matrix:
;                          	  $9121
;          $9120	b7  b6  b5  b4  b3  b2  b1  b0
; 	127-0111 1111 	F7  Hom  -   0   8   6   4   2
;               ASCII   ??  ??  45  48  56  54  52  50
;                 VIC   07  06  05  04  03  02  01  00
;
; 	191-1011 1111 	F5       @   O   U   T   E   Q
;               ASCII   ??  ??  64  79  85  84  69  81
;                 VIC   17  16  15  14  13  12  11  10
;
; 	223-1101 1111 	F3   =   :   K   H   F   S  Com
;               ASCII   ??  61  58  75  72  70  83  ??
;                 VIC   27  26  25  24  23  22  21  20
;
; 	239-1110 1111 	F1  RSh  .   M   B   C   Z  Spc
;               ASCII   ??  ??  46  77  66  67  90  32
;                 VIC   37  36  35  34  33  32  31  30
;
; 	247-1111 0111	Cur  /   ,   N   V   X  LSh Run
;               ASCII   ??  47  44  78  86  88  ??  ??
;                 VIC   47  46  45  44  43  42  41  40
;
; 	251-1111 1011	Cur  ;   L   J   G   D   A  Ctr
;               ASCII   ??  59  76  74  71  68  65  ??
;                 VIC   57  56  55  54  53  52  51  50
;
; 	253-1111 1101	Ret  *   P   I   Y   R   W
;               ASCII   13  42  80  73  89  82  87
;                 VIC   67  66  65  64  63  62  61  60
;
; 	254-1111 1110	Del Pnd  +   9   7   5   3   1
;               ASCII   08  ??  43  57  55  53  51  49
;                 VIC   77  76  75  74  73  72  71  70
;
;	 P =$65,O=$14,A=$51,Q=$10
;	SPC=$30,N=$44,M=$34
;	 1 =$70,2=$00,3=$71,4=$01
;
; Joysticks:
;
;               b7    b6    b5    b4    b3    b2    b1    b0
;	$9111	-     -     Fire  Left  Down  Up    -     -
;	$9120	Right -     -     -     -     -     -     -
;
;	$9122	DDRB = Data Direction Register port $9120
;               0 = input, 1 = output
;		Bit7 should be 0 to read joystick $9120
;
; Video registers:
;
;	$9000	horizontal centering
;		bits 0-6 horizontal centering
;		bit  7 sets interlace scan
;	$9001	vertical centering
;	$9002	set # of columns
;		bits 0-6 set # of columns
;		bit 7 is part of video matrix address
;	$9003	set # of rows
;		bit 0 sets 8x8 or 16x8 chars
;		bits 1-6 set # of rows
;	$9005	start of character memory
;		bits 0-3 start of character memory (default = 0)
;		bits 4-7 is rest of video address (default= F)
;		BITS 3,2,1,0 CM startinq address
;			     HEX   DEC
;		0000   ROM   8000  32768
;		0001	     8400  33792
;		0010	     8800  34816
;		0011	     8C00  35840
;		1000   RAM   0000  0000
;		1001  xxxx
;		1010  xxxx   unavail.
;		1011  xxxx
;		1100	     1000  4096
;		1101	     1400  5120
;		1110	     1800  6144
;		1111	     1C00  7168
;	$900f	Screen and border color register
;		bits 4-7 select background color
;		bits 0-2 select border color
;		bit 3 selects inverted or normal mode
;

; VIC constants

	OBJCHAR 	= 178			; first char nr for objects
	scrchar 	= 22			; char/line
	scrline		= 22			; lines/screen
	ScreenSize	= scrchar*scrline * 8	; Screen size bytes
	ScreenRowBytes	= scrchar * 16		; bytes/line
	SpriteMaxY	= 161			; maxlines - 16 + 1

; VIC variables:

	fontcol:	.byte 0		; Frontcolour text

; Addresses

	MapAddr		= $400		; RAM
	ScreenAddr	= $0200		; VIDEO RAM
	CharAddress	= $1000		; CHAR RAM
	Reg0		= $9000		; Horizontal centering
	Reg1		= $9001		; Vertical centering
	Reg2		= $9002		; Set # of columns
	Reg3		= $9003		; Set # of rows
	Reg5		= $9005		; Start of character memory
	RegF		= $900f		; Screen and border color register
	JoyAddr1	= $9111		; Joystick address 1
	KeyColAddr	= $9120		; Keyboard column (write)
	JoyAddr2	= $9120		; Joystick address 2
	KeyRowAddr	= $9121		; Keyboard row (read)
	DDRB		= $9122		; Bit7 must be 0 for joystick read
	Timer1_Low  	= $9114		; Timer 1 low byte
	Timer1_High 	= $9115		; Timer 1 high byte
	Timer2_Low  	= $9118		; Timer 2 low byte
	Timer2_High 	= $9119		; Timer 2 high byte
	AUX		= $911b		; Auxiliary control register
	IFR		= $911d		; Interrupt Flag Register
	IER		= $911e		; Interrupt Enable Register
	ColorAttr	= $9600		; Color attributes

;----------------------------------------------------------------------
; VIC20 video hardware fns
;----------------------------------------------------------------------

init:

; Disable interrupts

	lda #$7f
	sta $911e
	sta $912d
	sta $912e

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
scrinit:
	txa
	sta ScreenAddr,x
	inx
	cpx#242
	bne scrinit


; Reset fontcolor
	lda #0		; Black
	sta fontcol

; Init VIA timer for timeout 1/25 sec

	lda #$e0		; T1 continuous
	sta AUX
	lda #$7f		; No int
	sta IER
	lda #0
	sta IFR			; Clear interrupt flags
	lda #<timervalue	; Timer 1/25 sec
	sta Timer1_Low
	lda #>timervalue
	sta Timer1_High

	rts





;--------------------------------------------------------
; Keys
;
; Out: joyval=x65FUDLR (bit cleared if key pressed)
;             ||||||||
;             |||||||+> Right    KEY 0  - X
;             ||||||+-> Left     KEY 1  - Z
;             |||||+--> Down     KEY 2  - .
;             ||||+---> Up       KEY 3  - ;
;             |||+----> Fire1    KEY 4  - SPC
;             ||+-----> Fire2    KEY 5  - Q
;             |+------> Fire3    KEY 6  - P
;             +-------> Not used
;
;                       Option1  KEY 7  - 1
;                       Option2  KEY 8  - 2
;                       Option3  KEY 9  - 3
;                       Option4  KEY 10 - 4
;--------------------------------------------------------

jkeys:  .byte $42,$61,$68,$48,$62,$10,$37       ; Keys defined by game designer.
        .byte $30,$31,$11,$12                   ; menu options.

;----------------------------------------------------
; Wait for keypress.
;----------------------------------------------------

prskey:
	txa
	pha
prsloop:
	jsr vsync

;	lda #$79
;	ldx #$01
;	jsr OSBYTE

	cpx #$ff
	beq prsloop		; wait until key pressed
	txa
	jsr joykey
	pla
	tax
	rts

bbcinit:
;	sei
;	lda EVENTV
;	sta old_eventv
;	lda EVENTV+1
;	sta old_eventv+1

;	lda #<event_handler
;	sta EVENTV
;	lda #>event_handler
;	sta EVENTV+1
;	cli
	
	; Enable VSYNC event.
;	lda #14
;	ldx #4
;	jmp OSBYTE

	rts

bbckill:
;	sei
;	lda old_eventv
;	sta EVENTV
;	lda old_eventv+1
;	sta EVENTV+1
;	cli
	
	; Disable VSYNC event.
;	lda #13
;	ldx #4
;	jmp OSBYTE

	rts

old_eventv:
	.res 2

event_handler:
	php
	cmp #4
	bne not_vsync

	inc vsync_count

not_vsync:
	plp
	jmp (old_eventv)

bbcsync:
	lda vsync_count
	cmp #2
	bcc bbcsync
	lda #0
	sta vsync_count
	rts

;--------------------------------------------------------
; Keyboard test routine.
;
; Input:
;  A = key to read, high nibble=row and low nibble=col
;
; Output:
;  carry clr = key pressed
;  carry set = key not pressed
;--------------------------------------------------------

ktest:
	sta z80_a		; save key
	tya
	pha

; _BEEB keyboard read
	lda z80_a
	eor #$80		; _BEEB just look for this key
	tax

;	lda #$79
;	jsr OSBYTE

	txa
	bmi pressed 	; _BEEB X is negative if key is pressed

	sec			; key not pressed
	pla
	tay
	rts

pressed:
	clc			; key pressed
	pla
	tay
	rts

;---------------------------------------------------------------
; Getkey in column,row format
;
; Output:
;  A = high nibble=row and low nibble=column key in matrix
;---------------------------------------------------------------

kget:
	stx xtmp
kget1:
;	lda #$79			; _BEEB read keyboard with OSBYTE &79
;	ldx #$01
;	jsr OSBYTE

	cpx #$ff
	beq kget1

	txa
	pha
kget2:
;	lda #$79			; wait for released key
;	ldx #$01
;	jsr OSBYTE

	cpx #$ff
	bne kget2

	pla
	ldx xtmp
	rts

;----------------------------------------------------
; AtoMMC joystick controls.
; _BEEB TODO
;----------------------------------------------------

joyinit:
joysin:
	rts


;----------------------------------------------------
; Delay routine 1/50 sec
;
; Wait 1/60 sec = 16666 usec
; Wait 208 x 16 =  3328 usec
;                 19994 usec
; rts           =     6 usec
; Total         = 20000 usec
;----------------------------------------------------

delay:
	sta xtmp
del_loop:
;	lda #19
;	jsr OSBYTE		; wait for vsync _BEEB

	ldy #208		; wait 208 x 16 = 3328 usec
delay1:
	adc ($80,x)		;	 6 usec
	adc ($80,x)		;	 6 usec
	dey			;	 2 usec
	bne delay1		;	 2 usec
	nop			; 2 usec
	nop			; 2 usec
				; tot: 20000 usec
	dec xtmp
	bne del_loop
	rts

;-------------------------------------------------------------
; Screen synchronisation.
;
;  - read joystick/keyboard
;  - handle sound
;  - sync framerate with clock
;  - handle shrapnel every even frame
;-------------------------------------------------------------

vsync:
	pha
	tya
	pha
	txa
	pha
	jsr joykey		; read joystick/keyboard.
vsync1:
;	lda #19
;	jsr OSBYTE		; _BEEB vsync

	lda clock
	and #1
	bne:+
	jsr proshr		; handle shrapnel every even frame
:
	lda sndtyp
	beq sndskip

;-----------------------------------------------------------------------------------------------------------------------------------------------------
; beep effect using psg channel 0
;-----------------------------------------------------------------------------------------------------------------------------------------------------

	php			; save irq interrupt state

	sei			; disable interrupts
	
	lda #$0b		; disable keyboard output that drives bit 7 of via port A
	sta $fe40

	lda #$ff		; via port A bits 0 - 7 output
	sta $fe43

	lda #$90		; psg channel 0 full volume
	jsr psgWrite

beepLoop:

	lda sndtyp	; update low 4 bits of psg channel 0 tone register with 2 bits from sndtyp
	and #3
	asl a
	asl a
	ora #$80
	jsr psgWrite

	lda sndtyp	; update high 6 bits of psg channel 0 tone register with 6 bits from sndtyp
	lsr a
	lsr a
	jsr psgWrite

	ldy sndtyp		; do the half cycle delay

beepWait:

	dey
	bne beepWait
	
	dec sndtyp			; reduce delay by 1
	bne beepLoop		; repeat above until delay = 0
	
	lda #$9f				; channel 0 volume off
	jsr psgWrite

	lda #$7f		; via port A bit 7 input from keyboard output
	sta $fe4f
	
	lda #$03		; enable keyboard output
	sta $fe40
	
	plp			; restore irq state

;-----------------------------------------------------------------------------------------------------------------------------------------------------

sndskip: 
	pla
	tax
	pla
	tay
	pla
	rts

sndtyp:		.byte 0



;-----------------------------------------------------------------------------------------------------------------------------------------------------
; psgWrite 					write data to psg
;-----------------------------------------------------------------------------------------------------------------------------------------------------
; entry			A			data to be written to 76489 psg
;-----------------------------------------------------------------------------------------------------------------------------------------------------
; exit			A			destroyed
;-----------------------------------------------------------------------------------------------------------------------------------------------------

psgWrite:

	sta $fe4f				; place psg data on port a
	
	lda #0 + 0				; psg -we low
	sta $fe40
	
	pha					; 5uS delay
	pla
	nop
	nop
	
	lda #0 + 8				; psg -we high
	sta $fe40

	rts					; return



;----------------------------------------------------
; Draw sprite
;----------------------------------------------------
.if oflag
sprite:
	stx xtmp		; Save X-reg
	jsr scadd 		; get screen address in scraddr.

	lda dispx 		; x position.
	and #7 			; position straddling cells.
	sta z80_b		; store in b register.

	lda z80_l		; store sprite graphic address.
	sta sprit1+1
	sta sprit2+1
	lda z80_h
	sta sprit1+2
	sta sprit2+2

	ldx #0			; pixel height.
	ldy #0
sprit1:
	lda objdta,x		; fetch first byte.
	sta spr
	inx
sprit2:
	lda objdta,x
	sta spr+1

	lda #0
	sta spr+2
	jsr sprit7		; shift sprite

	lda spr			; fetch graphic.
	ldy #0			; _BEEB
	eor (scraddr),y		; merge with screen image.
	sta (scraddr),y		; write to screen.

	lda spr+1		; fetch graphic.
	ldy #8			; _BEEB
	eor (scraddr),y		; merge with screen image.
	sta (scraddr),y		; write to screen.

	lda spr+2		; fetch graphic.
	ldy #16			; _BEEB
	eor (scraddr),y		; merge with screen image.
	sta (scraddr),y		; write to screen.

	jsr nline

	inx			; next source byte.
	cpx #32
	bne sprit1		; repeat

	ldx xtmp		; retreive X-reg
	rts

;spr:	.byte 0,0,0
.endif
;------------------------------------------------------------------
; This routine returns a screen address for (dispx, dispy) in scraddr.
;------------------------------------------------------------------

scadd:
	lda dispx
	and #$f8
	sta scraddr
	lda #0
	sta scraddr+1
	asl scraddr
	rol scraddr+1

	ldx dispy
	clc
	lda scraddr
	adc SCADTB_lb,x
	sta scraddr
	lda scraddr+1
	adc SCADTB_hb,x
	sta scraddr+1


	rts

;-----------------------------------------------------------------
; These are the sprite routines.
; sspria = single sprite, old (ix).
; ssprib = single sprite, new (ix+5).
; sspric = both sprites, old (ix) and new (ix+5).
;-----------------------------------------------------------------

sspria:
	jsr gsprad		; get old sprite address.
sspri2:
.if rflag
	lda z80_e				; 3c
	sta dline_spraddr1+1	; 4c
	sta dline_spraddr2+1	; 4c
	lda z80_d				; 3c
	sta dline_spraddr1+2	; 4c
	sta dline_spraddr2+2	; 4c

	ldx sprshft
	lda shift_table,x
	sta dline_shift1+1
	sta dline_shift2+1
	lda shift_table+1,x
	sta dline_shift1+2
	sta dline_shift2+2

	ldx #0			; vertical lines.
sspri0:
	jsr dline		; draw a line.
  .if bflag
	cpx #48
  .else
	cpx #32
  .endif
	bne sspri0		; repeat 16 times x 2 bytes
	rts
.else
  .if bflag
	lda #24
  .else
	lda #16			; vertical lines.
  .endif
	sta acnt
sspri0:
	jsr dline		; draw a line.
	dec acnt
	bne sspri0		; repeat 16 times
	rts

acnt:	.byte 0
.endif

;-----------------------------------------------------------------

ssprib:
	jsr gspran 		; get new sprite address.
	jmp sspri2

;-----------------------------------------------------------------

sspric:
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.if rflag
	jsr gsprad 		; get old sprite address.

	lda z80_e				; 3c
	sta ddline_spraddr3+1	; 4c
	sta ddline_spraddr4+1	; 4c

	lda z80_d				; 3c
	sta ddline_spraddr3+2	; 4c
	sta ddline_spraddr4+2	; 4c

	ldx sprshft
	lda shift_table,x
	sta ddline_shift3+1
	sta ddline_shift4+1
	lda shift_table+1,x
	sta ddline_shift3+2
	sta ddline_shift4+2

	jsr exx  		; store addresses.
	jsr gspran 		; get new sprite addresses.

	lda z80_e				; 3c
	sta ddline_spraddr1+1	; 4c
	sta ddline_spraddr2+1	; 4c

	lda z80_d				; 3c
	sta ddline_spraddr1+2	; 4c
	sta ddline_spraddr2+2	; 4c

	ldx sprshft
	lda shift_table,x
	sta ddline_shift1+1
	sta ddline_shift2+1
	lda shift_table+1,x
	sta ddline_shift1+2
	sta ddline_shift2+2

	ldx #0
lloop:
	jsr ddline 		; draw a line.
  .if bflag
	cpx #48
  .else
	cpx #32
  .endif
	bne lloop
	rts

shift_table:
.word shift0
.word shift2
.word shift4
.word shift6

;-------------------------------------------------------------
; Drop through.
; Line drawn, now work out next target address.
;
; Input:
;  B  = right mask
;  C  = left mask
;  DE = spriteaddress
;  scraddr = screen address
;-------------------------------------------------------------

dline:

; first screen byte

dline_spraddr1:
	ldy $ffff,x 		; graphic data.
dline_shift1:
	lda $ffff,y
	sta sprtmp
	and z80_c 		; mask away what's not needed.

	ldy #0
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

; carry to next screen byte

	lda sprtmp		; fetch data.
	and z80_b 		; mask away unwanted
	sta sprtmp

; middle screen byte

	inx
dline_spraddr2:
	ldy $ffff,x 		; second bit of data.
dline_shift2:
	lda $ffff,y
	sta sprtmp2
	and z80_c 		; mask away what's not needed.
	ora sprtmp
	ldy #16
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

; carry to last screen byte

	lda sprtmp2
	and z80_b 		; mask away unwanted

	ldy #24
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

	inx
.else
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	jsr gsprad 		; get old sprite address.
	jsr exx  		; store addresses.
	jsr gspran 		; get new sprite addresses.

.if bflag
	lda #23
.else
	lda #15
.endif
	sta ccnt
lloop:
	jsr dline 		; draw a line.
	jsr exx  		; restore old addresses.
	jsr dline 		; delete a line.
	jsr exx  		; flip to new sprite addresses.
	dec ccnt
	bne lloop

	jsr dline		; draw a line.
	jsr exx 		; restore old addresses.

;-------------------------------------------------------------
; Drop through.
; Line drawn, now work out next target address.
;
; Input:
;  B  = right mask
;  C  = left mask
;  DE = spriteaddress
;  scraddr = screen address
;-------------------------------------------------------------

dline:
	ldy #0
	lda (z80_de),y 		; graphic data.
	pha
	and z80_c 		; mask away what's not needed.
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; left byte on screen.

; _BEEB screen arrangement
;	clc
;	lda scraddr
;	adc #16
;	sta scraddr
;	bcc :+
;	inc scraddr+1
;:
	pla
	and z80_b 		; mask away unwanted bits.
	ldy #24
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; right byte on screen

	inc z80_e 		; next graphic.
	bne :+
	inc z80_d
:
; _BEEB screen arrangement
;	sec
;	lda scraddr 		; one character cell to the left.
;	sbc #8
;	sta scraddr
;	lda scraddr+1
;	sbc #0
;	sta scraddr+1

	ldy #0
	lda (z80_de),y 		; second bit of data.
	ldy #16
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; middle byte on screen.

	inc z80_e 		; point to next line of data.
	bne :+
	inc z80_d
:
; _BEEB screen arrangement
;	sec
;	lda scraddr 		; another char left.
;	sbc #8
;	sta scraddr
;	lda scraddr+1
;	sbc #0
;	sta scraddr+1
.endif

;----------------------------------------------------------------------
; Line drawn, now work out next target address.
;----------------------------------------------------------------------

; _BEEB screen arrangement

nline:
	lda scraddr 			; get low byte of address.
	and #15				
	cmp #15				; is this last line of row?
	beq beeb_next_row

	; within same row
	inc scraddr			; new low byte of address.
	rts

beeb_next_row:
	clc
	lda scraddr
	adc #<(ScreenRowBytes-15)
	sta scraddr			; new low byte of address.
	lda scraddr+1
	adc #>(ScreenRowBytes-15)
	sta scraddr+1			; new high byte of address.
	rts

.if rflag
;-------------------------------------------------------------
; Drop through.
; Line drawn, now work out next target address.
;
; Input:
;  B  = right mask
;  C  = left mask
;  DE = spriteaddress
;  scraddr = screen address
;-------------------------------------------------------------

ddline:

; NEW SPRITE

; first screen byte

ddline_spraddr1:
	ldy $ffff,x 		; graphic data.
ddline_shift1:
	lda $ffff,y
	sta sprtmp
	and z80_c 			; mask away what's not needed.

	ldy #0
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

; carry to next screen byte

	lda sprtmp			; fetch data.
	and z80_b 			; mask away unwanted
	sta sprtmp

; middle screen byte

	inx
ddline_spraddr2:
	ldy $ffff,x 		; second bit of data.
ddline_shift2:
	lda $ffff,y
	sta sprtmp2
	and z80_c 			; mask away what's not needed.
	ora sprtmp

	ldy #16
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

; carry to last screen byte

	lda sprtmp2
	and z80_b 			; mask away unwanted

	ldy #24
	eor (scraddr),y 	; XOR with what's there.
	sta (scraddr),y 	; bung it in.

; _BEEB next row

	lda scraddr 		; get low byte of address.
	and #15				
	cmp #15				; is this last line of row?
	beq :+

	; within same row can't increment page
	inc scraddr			; new low byte of address.
	bne ddline2

:
	clc
	lda scraddr
	adc #<(ScreenRowBytes-15)
	sta scraddr			; new low byte of address.
	lda scraddr+1
	adc #>(ScreenRowBytes-15)
	sta scraddr+1		; new high byte of address.

; OLD SPRITE

ddline2:

; still first row but different data!

	dex

; first screen byte

ddline_spraddr3:
	ldy $ffff,x			; graphic data.
ddline_shift3:
	lda $ffff,y
	sta sprtmp
	and z80_cp 			; mask away what's not needed.

	ldy #0
	eor (z80_hlp),y 	; XOR with what's there.
	sta (z80_hlp),y 	; bung it in.

; carry to next screen byte

	lda sprtmp			; fetch data.
	and z80_bp 			; mask away unwanted
	sta sprtmp

; middle screen byte

	inx
ddline_spraddr4:
	ldy $ffff,x 		; second bit of data.
ddline_shift4:
	lda $ffff,y
	sta sprtmp2
	and z80_cp 			; mask away what's not needed.
	ora sprtmp

	ldy #16
	eor (z80_hlp),y 	; XOR with what's there.
	sta (z80_hlp),y 	; bung it in.

; last screen byte

	lda sprtmp2			; fetch data.
	and z80_bp 			; mask away unwanted

	ldy #24
	eor (z80_hlp),y 	; XOR with what's there.
	sta (z80_hlp),y 	; bung it in.

; next row of sprite data

	inx

; _BEEB screen arrangement

	lda z80_lp 			; get low byte of address.
	and #15				
	cmp #15				; is this last line of row?
	beq :+

	; within same row can't increment page
	inc z80_lp			; new low byte of address.
	rts

:
	clc
	lda z80_lp
	adc #<(ScreenRowBytes-15)
	sta z80_lp			; new low byte of address.
	lda z80_lp+1
	adc #>(ScreenRowBytes-15)
	sta z80_lp+1		; new high byte of address.
	rts
.endif

;----------------------------------------------------
; Display character in A at dispx,dispy.
;
; Input:
;  A 	   = character to print
;----------------------------------------------------

pchar:
	sta fntaddr
	lda #0
	sta fntaddr+1
	asl fntaddr  		; multiply char by 8.
	rol fntaddr+1
	asl fntaddr
	rol fntaddr+1
	asl fntaddr
	rol fntaddr+1

	lda fntaddr
	clc
	adc #<(FONT-256)
	sta fntaddr		; that's the low byte.
	lda fntaddr+1
	adc #>(FONT-256)
	sta fntaddr+1		; add displacement.
pchark:
	jsr gprad		; get screen address.
	ldx #7			; lines to write.
pchar0:
	ldy #0
	lda (fntaddr),y 	; get image byte.
	ldy scrtab,x		; Get rowoffset
.if iflag
	eor #TxtInvert		; Invert
.endif
	sta (scraddr),y 	; copy to screen.
	inc fntaddr		; next image byte.
	bne :+
	inc fntaddr+1
:
	dex			; next screen row down.
	bpl pchar0		; repeat.
	rts

scrtab:		.byte $07,$06,$05,$04,$03,$02,$01,$00
;scrtab:	.byte $e0,$c0,$a0,$80,$60,$40,$20,$00

.if rflag
;----------------------------------------------------
; Shift tables
;----------------------------------------------------

.align 256
shift0:
.repeat 256, i
	.byte i
.endrep

shift2:
.repeat 256, i
	.byte (i >> 2) | (i << 6) & $c0
.endrep

shift4:
.repeat 256, i
	.byte (i >> 4) | ((i << 4) & $f0)
.endrep

shift6:
.repeat 256, i
	.byte (i >> 6) | ((i << 2) & $fc)
.endrep
.endif
