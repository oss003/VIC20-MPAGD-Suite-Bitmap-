Files needed for CC65:

bbc.cfg		, memory config file
ld65.exe	, linker
ca65.exe	, assembler
MAKE.BAT	, batchfile

Files needed for MPAGD:

game.cfg	, flags created by AGD compiler
bbc.inc		, BBC platform depending routines
loader.inc	, SWRAM loader source
z80-zp.inc	, zero-page variable decalacration Z80 registers
engine-zp.inc	, zero-page variable declaration MPAGD engine
loader.bin	, SWRAM loader binary
game.asm	, MPAGD startfile
game.inc	, file which includes MPAGD assembler file
z80.asm		, variable declaration Z80 register/opcode emulation
