@echo off
imdisk -a -t file -m B: -f "%CD%\floppy.img"
if %ERRORLEVEL% neq 0 pause