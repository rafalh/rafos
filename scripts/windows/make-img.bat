@echo off
if exist env.bat call env.bat
cd ..\..
copy grub/grub-floppy.img bin/floppy.img

imdisk -a -t file -m B: -f "%CD%\bin\floppy.img"
if %ERRORLEVEL% neq 0 pause

mkdir B:\rafos
mkdir B:\rafos\system32
copy bin/kernel.bin B:\rafos\kernel.bin
copy grub/menu.lst B:\grub

imdisk -d -m B:
if %ERRORLEVEL% neq 0 pause
pause
