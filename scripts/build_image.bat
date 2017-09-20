@echo off
call mount
copy /V "bin\ntoskrnl.exe" "B:\rafos\system32\"
ping -n 3 127.0.0.1 >nul
call unmount
