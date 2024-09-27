; *******************************************************************************
; *******************************************************************************
; ** Definitions of KERNAL routines and memory locations                       **
; *******************************************************************************
; *******************************************************************************/


	; KERNAL page zero
	
status	= $90			; I/O status byte
verck	= $93			; load/verify flag
c3p0	= $94			; serial output: deferred character flag
bsour	= $95			; serial output: deferred character
xsav	= $97			; register save
ldtnd	= $98			; open file count
dfltn	= $99			; input device number
dflto	= $9a			; output device number
pcntr	= $a3			; serial EOI flag
firt	= $a4			; serial input byte
cntdn	= $a5			; serial bus bit count
sal	= $ac			; load/save current address low byte
sah	= $ad			; load/save current address high byte
eal	= $ae			; save end address low byte
eah	= $af			; save end address high byte
fnlen	= $b7			; file name length
la	= $b8			; logical file
sa	= $b9			; secondary address
fa	= $ba			; current device number
fnadr	= $bb			; file name pointer
stal	= $c1			; save start address low byte
stah	= $c2			; save start address low byte
memuss	= $c3			; load start address

	; KERNAL page 2

lat	= $0259			; logical file table
fat	= $0263			; device number table
sat	= $026d			; secondary address table


	; KERNAL routines

serout1	= $e4a0			; set serial DATA high
serout0	= $e4a9			; set serial DATA low
serget	= $e4b2			; get serial CLK status
scatn	= $eec5			; set serial ATN high
facptr	= $ef19			; input a byte from the serial bus
srclkhi	= $ef84			; set serial CLK high
srclklo	= $ef8d			; set serial CLK low
waitabit = $ef96		; 1ms delay
rspause	= $f160			; wait for RS-232 to be idle
fgetin	= $f1f5			; get a character from the input device
fchrin	= $f20e			; input character from channel
fndflno	= $f3cf			; find file table entry
setflch	= $f3df			; set file details from table,.X
fload2	= $f549			; default load vector contents
srching = $f647			; print "SEARCHING..."
ldvrmsg	= $f66a			; print "LOADING" or "VERIFYING"
fsave2	= $f685			; normal save vector target
saving	= $f728			; print "SAVING <file name>"
fe_2mnyf = $f77e		; display too many files
fe_alopn = $f781		; display file already open
fe_ntopn = $f784		; display file not open
fe_ntfnd = $f787		; display file not found
fe_dvntp = $f78a		; device not present error
fe_ntinp = $f78d		; display not input file
rd300	= $fbd2			; copy I/O start address to buffer address
vprty	= $fd11			; check read/write pointer
wrt62	= $fd1b			; increment read/write pointer
oriost	= $fe6a			; OR into I/O status byte
stop	= $ffe1			; scan the stop key
