#!/bin/bash
SELF_DIR=$(dirname "$0")
if [ -f "$SELF_DIR/env.sh" ]; then . "$SELF_DIR/env.sh"; fi
GRUB_DIR="$SELF_DIR/../../grub"
#dd if=/dev/zero of="$1" bs=512 count=2880
#mkdosfs "$1"
dd if="$GRUB_DIR/stage1" of="$1" bs=512 count=1 conv=notrunc
dd if="$GRUB_DIR/stage2" of="$1" bs=512 seek=1 conv=notrunc
