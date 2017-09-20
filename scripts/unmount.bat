@echo off
imdisk -d -m B:
if %ERRORLEVEL% neq 0 pause
