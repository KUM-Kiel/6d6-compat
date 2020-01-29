#!/bin/sh
CC='i486-linux-musl-gcc -static --static' AR='i486-linux-musl-ar' STRIP='i486-linux-musl-strip' rake clean package
