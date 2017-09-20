@echo off
if exist env.bat call env.bat
cd ..\..\build
mingw32-make
pause
