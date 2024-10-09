@echo off

rem Test if Multipaint picture exists

  if exist .\_FileConvert\%1.txt goto ok
  echo Picture %1.txt not found, check the _FileConvert folder ... 
  goto end

rem Convert Multipaint file to MPAGD files

:ok
  echo Converting Multipaint file to asm files ...
  .\Tools\convertbitmap .\_FileConvert\%1
  move /Y .\_FileConvert\%1?.inc .\CC65\Picture>nul

rem Create asm file for data

  cd .\CC65\Picture
  echo ;------------------------------------------------- >pic_data.asm
  echo ; General Multipaint picture converter >>pic_data.asm
  echo ; Mauro/KC 2024 >>pic_data.asm
  echo ;------------------------------------------------- >>pic_data.asm
  echo. >>pic_data.asm
  echo .org $1000-2 >>pic_data.asm
  echo. >>pic_data.asm
  echo start_asm: >>pic_data.asm
  echo 	.include "%1d.inc" >>pic_data.asm
  echo eind_asm: >>pic_data.asm

rem Create asm file for colour

  echo ;------------------------------------------------- >pic_col.asm
  echo ; General Multipaint picture converter >>pic_col.asm
  echo ; Mauro/KC 2024 >>pic_col.asm
  echo ;------------------------------------------------- >>pic_col.asm
  echo. >>pic_col.asm
  echo .org $9600-2 >>pic_col.asm
  echo. >>pic_col.asm
  echo start_asm: >>pic_col.asm
  echo 	.include "%1c.inc" >>pic_col.asm
  echo eind_asm: >>pic_col.asm

rem Assemble data and colour file

  call build pic_data 
  call build pic_col 
  move /Y pic_data ..\..\pictures\%1d>nul
  move /Y pic_col ..\..\pictures\%1c>nul
  del pic_*.*
  del %1*.*

  cd ..\..

:end
  echo Ready
