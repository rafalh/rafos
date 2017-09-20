#!/bin/bash
SELF_DIR=$(dirname "$0")
if [ -f "$SELF_DIR/env.sh" ]; then . "$SELF_DIR/env.sh"; fi
cd "$SELF_DIR/../.."
IMG_NAME=bin/floppy.img
cp grub/grub-floppy.img $IMG_NAME
mmd -i $IMG_NAME ::/rafos
mcopy -i $IMG_NAME bin/kernel.bin ::/rafos/kernel.bin
mcopy -i $IMG_NAME grub/menu.lst ::/grub
mdir -i $IMG_NAME ::/grub
mdir -i $IMG_NAME ::/rafos
