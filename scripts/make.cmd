@echo off
call config.cmd

cd src/ntoskrnl
set PATH=%PATH%;%MINGW_PATH%
%MAKE% CC=gcc
if %ERRORLEVEL% neq 0 (
	pause
	exit
)
cd ../..
call build_image.bat
if %ERRORLEVEL% neq 0 pause