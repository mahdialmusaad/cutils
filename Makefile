CC=gcc
AR=ar rcs
CFLAGS=-Wall -Wextra -pedantic -Werror -pedantic-errors -std=c89 -Wno-unsafe-buffer-usage -Oz -fno-asynchronous-unwind-tables -fno-ident -ffast-math -fno-stack-protector
DEFS=-DCU_SETTING_BUILDLIB=1

default: build/libcutils.a

all: build/test

build/test: build/libcutils.a
	$(CC) $(CFLAGS) test.c build/libcutils.a -o build/test

build/libcutils.a: build/cutils.o
	$(AR) build/libcutils.a build/cutils.o
	strip -d build/libcutils.a

build/cutils.o: | build
	$(CC) $(CFLAGS) $(DEFS) -c -o build/cutils.o cutils.c

build:
	mkdir build

clean:
	rm -rf ./build/*
