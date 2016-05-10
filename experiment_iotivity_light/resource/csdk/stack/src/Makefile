# makefile for iotlite stack.
#

PLATFORM=zephyr

TOP=../../../..
EXTOUT=$(TOP)/extout
CSDK=$(TOP)/resource/csdk

CC=gcc
MAKEDEPEND=gccmakedep	# makedepend doesn't seem to work here
CFLAGS= -Wall -std=c99
INCLUDES = -I$(CSDK)/stack/include \
	   -I$(CSDK)/stack/include/internal \
	   -I$(TOP)/extlibs/tinycbor/tinycbor/src 
CFILES= ocstack.c \
	ocserver.c \
	occlient.c \
	ocobserve.c \
	ocpayload.c \
	oic_string.c \
	ocresource.c \
	occlientcb.c \
	occollection.c \
	ocpayloadparse.c \
	ocserverrequest.c \
	ocpayloadconvert.c
COBJS= $(CFILES:.c=.o)
OUT= $(EXTOUT)/$(PLATFORM)/libstack.a

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $(CFG) $(INCLUDES) -c $< -o $@

all:	Makefile $(OUT)

$(OUT): $(COBJS)
	ar rcs $(OUT) $(COBJS)

depend:
	$(MAKEDEPEND) -- $(CFLAGS) -- $(CFG) $(INCLUDES) $(CFILES)

clean:
	rm -f $(COBJS) $(OUT) Makefile.bak

# DO NOT DELETE
ocstack.o: ocstack.c /usr/include/stdc-predef.h /usr/include/string.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h /usr/include/ctype.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/stdio.h \
 /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/include/stdlib.h /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocobserve.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/ocadaptation.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocglobals.h \
 /usr/include/x86_64-linux-gnu/sys/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h /usr/include/arpa/inet.h \
 /usr/include/netinet/in.h /usr/include/x86_64-linux-gnu/sys/socket.h \
 /usr/include/x86_64-linux-gnu/sys/uio.h \
 /usr/include/x86_64-linux-gnu/sys/types.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
 /usr/include/x86_64-linux-gnu/bits/uio.h \
 /usr/include/x86_64-linux-gnu/bits/socket.h \
 /usr/include/x86_64-linux-gnu/bits/socket_type.h \
 /usr/include/x86_64-linux-gnu/bits/sockaddr.h \
 /usr/include/x86_64-linux-gnu/asm/socket.h \
 /usr/include/asm-generic/socket.h \
 /usr/include/x86_64-linux-gnu/asm/sockios.h \
 /usr/include/asm-generic/sockios.h \
 /usr/include/x86_64-linux-gnu/bits/in.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h
ocserver.o: ocserver.c /usr/include/stdc-predef.h /usr/include/string.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h /usr/include/ctype.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/wchar.h /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocadaptation.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocobserve.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/ocglobals.h \
 ../../../../resource/csdk/stack/include/ocstackextra.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 /usr/include/x86_64-linux-gnu/sys/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h /usr/include/arpa/inet.h \
 /usr/include/netinet/in.h /usr/include/x86_64-linux-gnu/sys/socket.h \
 /usr/include/x86_64-linux-gnu/sys/uio.h \
 /usr/include/x86_64-linux-gnu/sys/types.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
 /usr/include/x86_64-linux-gnu/bits/uio.h \
 /usr/include/x86_64-linux-gnu/bits/socket.h \
 /usr/include/x86_64-linux-gnu/bits/socket_type.h \
 /usr/include/x86_64-linux-gnu/bits/sockaddr.h \
 /usr/include/x86_64-linux-gnu/asm/socket.h \
 /usr/include/asm-generic/socket.h \
 /usr/include/x86_64-linux-gnu/asm/sockios.h \
 /usr/include/asm-generic/sockios.h \
 /usr/include/x86_64-linux-gnu/bits/in.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h
occlient.o: occlient.c /usr/include/stdc-predef.h /usr/include/string.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h /usr/include/ctype.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocadaptation.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/ocstack.h /usr/include/stdio.h \
 /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocobserve.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/jutility.h \
 ../../../../resource/csdk/stack/include/internal/ocglobals.h \
 /usr/include/x86_64-linux-gnu/sys/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h /usr/include/arpa/inet.h \
 /usr/include/netinet/in.h /usr/include/x86_64-linux-gnu/sys/socket.h \
 /usr/include/x86_64-linux-gnu/sys/uio.h \
 /usr/include/x86_64-linux-gnu/sys/types.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
 /usr/include/x86_64-linux-gnu/bits/uio.h \
 /usr/include/x86_64-linux-gnu/bits/socket.h \
 /usr/include/x86_64-linux-gnu/bits/socket_type.h \
 /usr/include/x86_64-linux-gnu/bits/sockaddr.h \
 /usr/include/x86_64-linux-gnu/asm/socket.h \
 /usr/include/asm-generic/socket.h \
 /usr/include/x86_64-linux-gnu/asm/sockios.h \
 /usr/include/asm-generic/sockios.h \
 /usr/include/x86_64-linux-gnu/bits/in.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h
ocobserve.o: ocobserve.c /usr/include/stdc-predef.h /usr/include/string.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/xlocale.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 /usr/include/stdio.h /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/include/stdlib.h /usr/include/x86_64-linux-gnu/bits/waitflags.h \
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
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocobserve.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h \
 ../../../../resource/csdk/stack/include/internal/jutility.h
ocpayload.o: ocpayload.c /usr/include/stdc-predef.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/stdlib.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/x86_64-linux-gnu/sys/types.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h /usr/include/stdio.h \
 /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 /usr/include/string.h /usr/include/xlocale.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h
oic_string.o: oic_string.c /usr/include/stdc-predef.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h /usr/include/string.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h /usr/include/xlocale.h \
 /usr/include/assert.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 /usr/include/stdio.h /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
ocresource.o: ocresource.c /usr/include/stdc-predef.h \
 /usr/include/string.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocobserve.h \
 ../../../../resource/csdk/stack/include/internal/occollection.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h
occlientcb.o: occlientcb.c /usr/include/stdc-predef.h
occollection.o: occollection.c /usr/include/stdc-predef.h \
 /usr/include/string.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 ../../../../resource/csdk/stack/include/ocstack.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/include/stdlib.h /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/payload_logging.h \
 ../../../../resource/csdk/stack/include/internal/occollection.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h
ocpayloadparse.o: ocpayloadparse.c /usr/include/stdc-predef.h \
 /usr/include/string.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 /usr/include/stdio.h /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h ../../../../extlibs/tinycbor/tinycbor/src/cbor.h \
 /usr/include/assert.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h \
 /usr/include/limits.h /usr/include/x86_64-linux-gnu/bits/posix1_lim.h \
 /usr/include/x86_64-linux-gnu/bits/local_lim.h \
 /usr/include/linux/limits.h \
 /usr/include/x86_64-linux-gnu/bits/posix2_lim.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h
ocserverrequest.o: ocserverrequest.c /usr/include/stdc-predef.h \
 /usr/include/string.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/xlocale.h ../../../../resource/csdk/stack/include/ocstack.h \
 /usr/include/stdio.h /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/include/stdlib.h /usr/include/x86_64-linux-gnu/bits/waitflags.h \
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
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/ocrandom.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/jutility.h
ocpayloadconvert.o: ocpayloadconvert.c /usr/include/stdc-predef.h \
 ../../../../resource/csdk/stack/include/platform_features.h \
 /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/x86_64-linux-gnu/sys/types.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
 ../../../../resource/csdk/stack/include/oic_malloc.h \
 /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/wchar.h /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
 ../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../resource/csdk/stack/include/ocrandom.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h /usr/include/xlocale.h \
 ../../../../resource/csdk/stack/include/internal/ocresourcehandler.h \
 ../../../../resource/csdk/stack/include/ocstack.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../resource/csdk/stack/include/internal/ocstackinternal.h \
 ../../../../resource/csdk/stack/include/internal/occlientcb.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../resource/csdk/stack/include/octypes.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../extlibs/tinycbor/tinycbor/src/cbor.h /usr/include/assert.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h \
 /usr/include/limits.h /usr/include/x86_64-linux-gnu/bits/posix1_lim.h \
 /usr/include/x86_64-linux-gnu/bits/local_lim.h \
 /usr/include/linux/limits.h \
 /usr/include/x86_64-linux-gnu/bits/posix2_lim.h /usr/include/string.h \
 /usr/include/setjmp.h /usr/include/x86_64-linux-gnu/bits/setjmp.h \
 ../../../../resource/csdk/stack/include/internal/ocresource.h \
 ../../../../resource/csdk/stack/include/internal/ocserverrequest.h \
 ../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadcbor.h \
 ../../../../resource/csdk/stack/include/internal/ocpayloadint.h
