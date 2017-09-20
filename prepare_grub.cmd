@echo off
call config.cmd
copy floppy.img grub.img
%DD% if=grub\stage1 of=grub.img bs=512 count=1
%DD% if=grub\stage2 of=grub.img bs=512 seek=1
pause
