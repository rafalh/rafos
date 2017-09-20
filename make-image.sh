#!/bin/bash
IMG_NAME=bin/floppy.img
cp grub/grub-floppy.img $IMG_NAME
mmd -i $IMG_NAME ::/rafos
mmd -i $IMG_NAME ::/rafos/system32
mcopy -i $IMG_NAME bin/ntoskrnl.exe ::/rafos/system32
mcopy -i $IMG_NAME grub/menu.lst ::/grub
mdir -i $IMG_NAME ::/grub
mdir -i $IMG_NAME ::/rafos/system32
