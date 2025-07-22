#!/bin/sh
CC="zig cc --target=x86_64-macos-none" AR="zig ar" rake clean package
