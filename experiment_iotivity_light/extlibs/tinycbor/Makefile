# make tinycbor for non-linux (e.g. zephyr)
#

PLATFORM=zephyr

TOP=../..
EXTOUT=$(TOP)/extout
SRC=tinycbor/src

CC=gcc
MAKEDEPEND= gccmakedep  # makedepend doesn't seem to work here
CFLAGS= -std=c99 -Wall
INCLUDES=
CFILES= $(SRC)/cborparser.c $(SRC)/cborencoder.c
COBJS= $(CFILES:.c=.o)
OUT=$(EXTOUT)/$(PLATFORM)/libcbor.a

.SUFFIXES: .c

all:	$(OUT)

$(SRC)/cborencoder.o: $(SRC)/cborencoder.c
	$(CC) $(CFLAGS) $(CFG) $(INCLUDES) -c $< -o $@

$(SRC)/cborparser.o: $(SRC)/cborparser.c
	$(CC) $(CFLAGS) $(CFG) $(INCLUDES) -c $< -o $@

$(OUT): $(SRC)/cborencoder.o $(SRC)/cborparser.o
	ar rcs $(OUT) $(COBJS)

depend: 
	$(MAKEDEPEND) -- $(CFLAGS) -- $(INCLUDES) $(CFILES)

clean:
	rm -f $(COBJS) $(OUT) Makefile.bak

# DO NOT DELETE
cborparser.o: tinycbor/src/cborparser.c /usr/include/stdc-predef.h \
 tinycbor/src/cbor.h /usr/include/assert.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h \
 /usr/include/limits.h /usr/include/x86_64-linux-gnu/bits/posix1_lim.h \
 /usr/include/x86_64-linux-gnu/bits/local_lim.h \
 /usr/include/linux/limits.h \
 /usr/include/x86_64-linux-gnu/bits/posix2_lim.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 tinycbor/src/cborconstants_p.h tinycbor/src/compilersupport_p.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/float.h /usr/include/math.h \
 /usr/include/x86_64-linux-gnu/bits/huge_val.h \
 /usr/include/x86_64-linux-gnu/bits/huge_valf.h \
 /usr/include/x86_64-linux-gnu/bits/huge_vall.h \
 /usr/include/x86_64-linux-gnu/bits/inf.h \
 /usr/include/x86_64-linux-gnu/bits/nan.h \
 /usr/include/x86_64-linux-gnu/bits/mathdef.h \
 /usr/include/x86_64-linux-gnu/bits/mathcalls.h \
 tinycbor/src/extract_number_p.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/x86_64-linux-gnu/sys/types.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 tinycbor/src/assert_p.h
cborencoder.o: tinycbor/src/cborencoder.c /usr/include/stdc-predef.h \
 tinycbor/src/cbor.h /usr/include/assert.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h \
 /usr/include/limits.h /usr/include/x86_64-linux-gnu/bits/posix1_lim.h \
 /usr/include/x86_64-linux-gnu/bits/local_lim.h \
 /usr/include/linux/limits.h \
 /usr/include/x86_64-linux-gnu/bits/posix2_lim.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 tinycbor/src/cborconstants_p.h tinycbor/src/compilersupport_p.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/float.h /usr/include/math.h \
 /usr/include/x86_64-linux-gnu/bits/huge_val.h \
 /usr/include/x86_64-linux-gnu/bits/huge_valf.h \
 /usr/include/x86_64-linux-gnu/bits/huge_vall.h \
 /usr/include/x86_64-linux-gnu/bits/inf.h \
 /usr/include/x86_64-linux-gnu/bits/nan.h \
 /usr/include/x86_64-linux-gnu/bits/mathdef.h \
 /usr/include/x86_64-linux-gnu/bits/mathcalls.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/x86_64-linux-gnu/sys/types.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 tinycbor/src/assert_p.h
