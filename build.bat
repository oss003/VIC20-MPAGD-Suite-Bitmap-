@echo off

 set model=0
 if "%2"=="pal" set model=1
 if "%3"=="pal" set model=1
 if "%4"=="pal" set model=1
 if "%5"=="pal" set model=1
 if "%6"=="pal" set model=1

 if "%2"=="ntsc" set model=0
 if "%3"=="ntsc" set model=0
 if "%4"=="ntsc" set model=0
 if "%5"=="ntsc" set model=0
 if "%6"=="ntsc" set model=0

rem Compile AGD file
 copy scripts\%1.agd agd
 if errorlevel 1 goto error
 cd AGD
 agd %1 %2 %3 %4 %5 %6

 copy %1.inc ..\cc65\ >nul
 copy game.cfg ..\cc65\ >nul
 del %1.*
 del game.cfg

rem Assemble file
 cd ..\cc65
 call make %1 %model% %2 %3 %4 %5 %6 %7 %8 %9
 copy %1.bin "..\GTK3VICE-3.8-win64\TAPES\%1.prg"
 del %1.bin

rem Create discimage 
 echo.
 echo Creating diskimage %1.d64
 cd ..\GTK3VICE-3.8-win64\bin
 c1541 -format "diskimage,id" d64 %1.d64 
 c1541 -attach %1.d64 -write ..\TAPES\%1.prg %1.prg
 move %1.d64 ..\discs >nul

rem Start emulator
 echo Starting Vice with %1.prg

rem cd ..\GTK3VICE-3.8-win64\bin
rem xvic -silent -memory all -ntsc ..\TAPES\%1.prg

 xvic -silent -memory all -ntsc "..\discs\%1.d64:%1.prg"
 cd ..\..

 goto end

:error
 echo %1.agd not found .....

:end
