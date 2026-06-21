# ---------------------------- User options ----------------------------

# Compiler to use.
CC=gcc

# Adjust as you see fit. Original is designed for minimum size.
CFLAGS=-Wall -Wextra -pedantic -std=c89 -Oz -fno-asynchronous-unwind-tables -fno-ident -ffast-math -fno-stack-protector

# You can selectively disable function groups by defining its specific macro to 0, e.g. 'CU_SETTING_NETWORK_FUNCS=0'
# A full list of all settings you can change can be found in the 'User Settings' section of cutils.h.
# '-D' is automatically prefixed to all elements.
DEFS=NDEBUG _FILE_OFFSET_BITS=64

# Extra arguments to pass to compiler.
EARG=

# ----------------------------------------------------------------------

DEFS:=$(addprefix -D, $(DEFS))
EXM=$(wildcard examples/*.c)
CFLAGS+=$(EARG)

static: build/libcutils.a
tests: build/test
examples: $(EXM)
all: static tests examples

$(EXM): build/libcutils.a
	$(CC) $(CFLAGS) $(DEFS) -DCU_SETTING_QUIET=1 $@ $< -o build/$(basename $(notdir $@))
	strip build/$(basename $(notdir $@))

build/test: build/libcutils.a
	$(CC) $(CFLAGS) $(DEFS) -DCU_SETTING_QUIET=1 tests/test.c $< -o $@
	strip $@

build/libcutils.a: build/cutils.o
	ar rcs $@ $<
	strip -x $@

build/cutils.o: | build
	$(CC) $(CFLAGS) $(DEFS) cutils.c -c -o $@

build:
	mkdir build

clean:
	rm -rf ./build
