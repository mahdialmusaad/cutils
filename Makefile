# ---------------------------- User options ----------------------------

# Compiler to use.
CC=gcc

# Normal flags to pass to compiler.
# Adjust as you see fit. Original is designed for minimum size.
CFLAGS=-Wall -Wextra -pedantic -std=c89 -Oz -fno-asynchronous-unwind-tables -fno-ident -fno-stack-protector

# Any macro definitions to pass to compiler.
# You can selectively disable function groups by defining its specific macro to 0, e.g. 'CU_SETTING_NETWORK_FUNCS=0'
# A full list of all settings you can change can be found in the 'User Settings' section of cutils.h.
DEFS=-DNDEBUG -D_FILE_OFFSET_BITS=64

# Any libraries to pass to compiler. 
LIBS=

# ----------------------------------------------------------------------

EXT=
EARG=
EXM=$(wildcard examples/*.c)
CFLAGS+=$(EARG)

ifneq (,$(filter $(CC),x86_64-w64-mingw32-gcc i686-w64-mingw32-gcc))
	LIBS+=-lws2_32 -liphlpapi -lpowrprof
	CFLAGS+=-Wno-format -Wno-long-long
	EXT=.exe
endif

static: build/libcutils.a
tests: build/test
examples: $(EXM)
all: static tests examples

$(EXM): build/libcutils.a
	$(CC) $@ $< $(CFLAGS) $(LIBS) $(DEFS) -DCU_SETTING_QUIET=1 -o build/$(basename $(notdir $@))$(EXT)

build/test: build/libcutils.a
	$(CC) tests/test.c $< $(CFLAGS) $(LIBS) $(DEFS) -DCU_SETTING_QUIET=1 -o $@$(EXT)

build/libcutils.a: build/cutils.o
	ar rcs $@ $<
	strip -x $@

build/cutils.o: cutils.c | build
	$(CC) cutils.c $(CFLAGS) $(LIBS) $(DEFS) -c -o $@

build:
	mkdir build

clean:
	rm -rf ./build
