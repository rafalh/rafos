@echo off
if exist env.bat call env.bat
cd ..\..\bochs
bochs -q -rc bochs-debug.txt
