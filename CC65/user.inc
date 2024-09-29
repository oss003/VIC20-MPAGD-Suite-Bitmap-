music_init	= $a800
music_on	= $a803
music_off	= $a806
music_play	= $a809

api:
	cmp #0
	bne api1
	jmp music_init
api1:
	cmp #1
	bne api2
	jmp music_on
api2:
	cmp #2
	bne api3
	jmp music_off
api3:
	cmp #3
	bne api_end
	jmp music_play
api_end:
	rts
