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
	bne snd_1
	jmp music_play
snd_1:
	cmp #5
	bne snd_2
	lda #222
	sta SetSoundVars
	rts
snd_2:
	cmp #6
	bne api_end
	lda #222
	sta SetSoundVars+3
	rts
api_end:
	rts
