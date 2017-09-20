@echo off
if exist env.bat call env.bat
cd ..\..
tools\windows\premake5 gmake
pause
