#!/bin/bash
SELF_DIR=$(dirname "$0")
if [ -f "$SELF_DIR/env.sh" ]; then . "$SELF_DIR/env.sh"; fi
PREFIX=i686-w64-mingw32-
CC=${PREFIX}gcc CXX=${PREFIX}g++ make -C "$SELF_DIR/../../build"
