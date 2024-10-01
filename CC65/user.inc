music_init	= $a900
music_on	= $a903
music_off	= $a906
music_play	= $a909

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
