CC=clang
AR=ar rcs

# Adjust as you see fit. Original is designed for minimum size.
CFLAGS=-Wall -Wextra -pedantic -std=c89 -Oz -fno-asynchronous-unwind-tables -fno-ident -ffast-math -fno-stack-protector
# You can selectively disable function groups by defining its specific macro to 0, e.g. -DCU_SETTING_NETWORK_FUNCS=0
DEFS=-DNDEBUG

static: build/libcutils.a
tests: build/cutils_test build/cutils_network_test
all: static tests

build/cutils_test: build/libcutils.a
	$(CC) $(CFLAGS) $(DEFS) test.c $< -o $@

build/cutils_network_test: build/libcutils.a
	$(CC) $(CFLAGS) $(DEFS) net_test.c $< -o $@

build/libcutils.a: build/cutils.o
	$(AR) $@ $<
	strip -x $@

build/cutils.o: | build
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ cutils.c

build:
	mkdir build

clean:
	rm -rf ./build/*
