#!/bin/sh
CC='x86_64-linux-musl-gcc -static --static' AR='x86_64-linux-musl-ar' STRIP='x86_64-linux-musl-strip' rake clean default package
