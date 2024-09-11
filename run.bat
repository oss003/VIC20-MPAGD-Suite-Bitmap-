@echo off

REM Locate your BeebAsm & b-em exes here

 set BEEBASM=beebasm\beebasm.exe
 set BEM=b-em\b-em.exe

REM Check if title- or panel screen exists

 if exist "screens/titles/%1.SCR" (
   echo - Titlescreen %1.scr found ...
   echo PUTFILE "screens/titles/%1.SCR", "TITLE", $5800, $5800 > disc.asm
 
   if exist "screens/panels/%1.PAN" (
     echo - Panelscreen %1.pan found ...
     echo PUTFILE "screens/LOADERPAN.BAS", "LOADER", $1900, $8023 >> disc.asm
     echo PUTFILE "screens/panels/%1.PAN", "PANEL", $6800, $6800 >> disc.asm
   ) else (
     echo PUTFILE "screens/LOADERSCR.BAS", "LOADER", $1900, $8023 >> disc.asm
   )
 ) else (
     echo PUTFILE "screens/LOADER.BAS", "LOADER", $1900, $8023 > disc.asm
 )

REM Check if game needs SWRAM or not

 IF EXIST Output/AGD002 goto swr

REM Game doesn't use SWRAM

:noswr
 IF NOT EXIST Output/%1.bin goto error
 echo PUTFILE "Output/%1.bin", "AGDGAME", $2600, $2600 >> disc.asm
 echo PUTTEXT "screens/BOOT", "!BOOT", $0000, $FFFF >> disc.asm
 %BEEBASM% -i disc.asm -do Discs\%1.ssd -opt 3 -title %1
 goto skip

REM Game uses SWRAM

:swr
 IF NOT EXIST Output/AGD001 goto error
 echo PUTFILE "Output/AGD001", "AGDRAM", $2600, $2600>> disc.asm
 echo PUTFILE "Output/AGD002", "AGDSWR", $4000, $4000>>disc.asm
 echo PUTFILE "cc65/loader.bin", "AGDGAME", $2100, $2100>>disc.asm
 echo PUTFILE "screens/BOOT", "!BOOT", $0000, $FFFF >> disc.asm
 %BEEBASM% -i disc.asm -do Discs\%1.ssd -opt 3 -title %1
 del Output\AGD001
 del Output\AGD002

:skip
 del disc.asm

REM Fire up b-em with the disc image

 %BEM% Discs\%1.ssd
 goto end

:error
 
 echo Output\%1.bin not found.

:end
