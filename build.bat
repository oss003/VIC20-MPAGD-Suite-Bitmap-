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

rem Start emulator
 echo Starting Vice with %1.prg
 cd ..\GTK3VICE-3.8-win64\bin

 xvic -silent -memory all -ntsc ..\TAPES\%1.prg
 cd..\..

 goto end

:error
 echo %1.agd not found .....

:end
