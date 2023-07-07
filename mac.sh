#!/bin/sh
CHAIN="x86_64-apple-darwin12"
CC="$CHAIN-gcc" AR="$CHAIN-ar" STRIP="$CHAIN-strip" rake clean package
