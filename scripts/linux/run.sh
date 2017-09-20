#!/bin/bash
SELF_DIR=$(dirname "$0")
if [ -f "$SELF_DIR/env.sh" ]; then . "$SELF_DIR/env.sh"; fi
cd "$SELF_DIR/../../bochs"
bochs -q -rc bochs-debug.txt
