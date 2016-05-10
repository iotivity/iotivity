# makefile for iotlite stack.
#

PLATFORM=zephyr

TOP=../../../../..
EXTOUT=$(TOP)/extout
CSDK=$(TOP)/resource/csdk

CC=g++
MAKEDEPEND= gccmakedep	# makedepend doesn't seem to work here
CFLAGS= -std=c++0x -Wall
LIBS= $(EXTOUT)/$(PLATFORM)/libstack.a \
      $(EXTOUT)/$(PLATFORM)/libcbor.a \
      $(EXTOUT)/$(PLATFORM)/lib$(PLATFORM).a
INCLUDES= -I$(CSDK)/stack/include \
	  -I$(CSDK)/stack/include/internal \
	  -I$(CSDK)/connectivity/inc
CFILES= ocserverbasicops.cpp \
	ocservercoll.cpp \
	ocserver.cpp \
	ocserverslow.cpp
COBJS=	$(CFILES:.cpp=.o)
PROGS=  $(CFILES:.cpp=)

.SUFFIXES: .c

# The samples missing here use pthread
all:	ocserverbasicops ocserverslow  # $(PROGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

ocserverbasicops: ocserverbasicops.o $(LIBS)
	$(CC) -o $@ $^

ocservercoll: ocservercoll.o $(LIBS)
	$(CC) -o $@ $^

ocserver: ocserver.o $(LIBS)
	$(CC) -o $@ $^

ocserverslow: ocserverslow.o $(LIBS)
	$(CC) -o $@ $^

depend: 
	$(MAKEDEPEND) -- $(CFLAGS) -- $(INCLUDES) $(CFILES)

clean:
	rm -f $(COBJS) $(PROGS) Makefile.bak

# DO NOT DELETE
ocserverbasicops.o: ocserverbasicops.cpp /usr/include/stdc-predef.h \
 /usr/include/stdio.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/stdlib.h \
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
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h /usr/include/unistd.h \
 /usr/include/x86_64-linux-gnu/bits/posix_opt.h \
 /usr/include/x86_64-linux-gnu/bits/environments.h \
 /usr/include/x86_64-linux-gnu/bits/confname.h /usr/include/getopt.h \
 /usr/include/signal.h /usr/include/x86_64-linux-gnu/bits/signum.h \
 /usr/include/x86_64-linux-gnu/bits/siginfo.h \
 /usr/include/x86_64-linux-gnu/bits/sigaction.h \
 /usr/include/x86_64-linux-gnu/bits/sigcontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigstack.h \
 /usr/include/x86_64-linux-gnu/sys/ucontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigthread.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/x86_64-linux-gnu/bits/sched.h \
 /usr/include/x86_64-linux-gnu/bits/timex.h \
 /usr/include/x86_64-linux-gnu/bits/setjmp.h \
 ../../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../../resource/csdk/stack/include/octypes.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../../resource/csdk/stack/include/oc_console_logger.h \
 ocserverbasicops.h \
 ../../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h
ocservercoll.o: ocservercoll.cpp /usr/include/stdc-predef.h \
 /usr/include/stdio.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/c++/4.8/string \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++config.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/os_defines.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/cpu_defines.h \
 /usr/include/c++/4.8/bits/stringfwd.h \
 /usr/include/c++/4.8/bits/memoryfwd.h \
 /usr/include/c++/4.8/bits/char_traits.h \
 /usr/include/c++/4.8/bits/stl_algobase.h \
 /usr/include/c++/4.8/bits/functexcept.h \
 /usr/include/c++/4.8/bits/exception_defines.h \
 /usr/include/c++/4.8/bits/cpp_type_traits.h \
 /usr/include/c++/4.8/ext/type_traits.h \
 /usr/include/c++/4.8/ext/numeric_traits.h \
 /usr/include/c++/4.8/bits/stl_pair.h /usr/include/c++/4.8/bits/move.h \
 /usr/include/c++/4.8/bits/concept_check.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_types.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_funcs.h \
 /usr/include/c++/4.8/debug/debug.h \
 /usr/include/c++/4.8/bits/stl_iterator.h \
 /usr/include/c++/4.8/bits/postypes.h /usr/include/c++/4.8/cwchar \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 /usr/include/c++/4.8/bits/allocator.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++allocator.h \
 /usr/include/c++/4.8/ext/new_allocator.h /usr/include/c++/4.8/new \
 /usr/include/c++/4.8/exception \
 /usr/include/c++/4.8/bits/atomic_lockfree_defines.h \
 /usr/include/c++/4.8/bits/localefwd.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++locale.h \
 /usr/include/c++/4.8/clocale /usr/include/locale.h \
 /usr/include/x86_64-linux-gnu/bits/locale.h /usr/include/c++/4.8/iosfwd \
 /usr/include/c++/4.8/cctype /usr/include/ctype.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/c++/4.8/bits/ostream_insert.h \
 /usr/include/c++/4.8/bits/cxxabi_forced.h \
 /usr/include/c++/4.8/bits/stl_function.h \
 /usr/include/c++/4.8/backward/binders.h \
 /usr/include/c++/4.8/bits/range_access.h \
 /usr/include/c++/4.8/bits/basic_string.h \
 /usr/include/c++/4.8/ext/atomicity.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/gthr.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/gthr-default.h \
 /usr/include/pthread.h /usr/include/sched.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/sched.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/bits/timex.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
 /usr/include/x86_64-linux-gnu/bits/setjmp.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/atomic_word.h \
 /usr/include/c++/4.8/bits/basic_string.tcc /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h \
 /usr/include/x86_64-linux-gnu/sys/types.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h /usr/include/unistd.h \
 /usr/include/x86_64-linux-gnu/bits/posix_opt.h \
 /usr/include/x86_64-linux-gnu/bits/environments.h \
 /usr/include/x86_64-linux-gnu/bits/confname.h /usr/include/getopt.h \
 /usr/include/signal.h /usr/include/x86_64-linux-gnu/bits/signum.h \
 /usr/include/x86_64-linux-gnu/bits/siginfo.h \
 /usr/include/x86_64-linux-gnu/bits/sigaction.h \
 /usr/include/x86_64-linux-gnu/bits/sigcontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigstack.h \
 /usr/include/x86_64-linux-gnu/sys/ucontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigthread.h \
 ../../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 ../../../../../resource/csdk/stack/include/octypes.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h
ocserver.o: ocserver.cpp /usr/include/stdc-predef.h /usr/include/stdio.h \
 /usr/include/features.h /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/c++/4.8/string \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++config.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/os_defines.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/cpu_defines.h \
 /usr/include/c++/4.8/bits/stringfwd.h \
 /usr/include/c++/4.8/bits/memoryfwd.h \
 /usr/include/c++/4.8/bits/char_traits.h \
 /usr/include/c++/4.8/bits/stl_algobase.h \
 /usr/include/c++/4.8/bits/functexcept.h \
 /usr/include/c++/4.8/bits/exception_defines.h \
 /usr/include/c++/4.8/bits/cpp_type_traits.h \
 /usr/include/c++/4.8/ext/type_traits.h \
 /usr/include/c++/4.8/ext/numeric_traits.h \
 /usr/include/c++/4.8/bits/stl_pair.h /usr/include/c++/4.8/bits/move.h \
 /usr/include/c++/4.8/bits/concept_check.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_types.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_funcs.h \
 /usr/include/c++/4.8/debug/debug.h \
 /usr/include/c++/4.8/bits/stl_iterator.h \
 /usr/include/c++/4.8/bits/postypes.h /usr/include/c++/4.8/cwchar \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 /usr/include/c++/4.8/bits/allocator.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++allocator.h \
 /usr/include/c++/4.8/ext/new_allocator.h /usr/include/c++/4.8/new \
 /usr/include/c++/4.8/exception \
 /usr/include/c++/4.8/bits/atomic_lockfree_defines.h \
 /usr/include/c++/4.8/bits/localefwd.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++locale.h \
 /usr/include/c++/4.8/clocale /usr/include/locale.h \
 /usr/include/x86_64-linux-gnu/bits/locale.h /usr/include/c++/4.8/iosfwd \
 /usr/include/c++/4.8/cctype /usr/include/ctype.h /usr/include/endian.h \
 /usr/include/x86_64-linux-gnu/bits/endian.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap.h \
 /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
 /usr/include/c++/4.8/bits/ostream_insert.h \
 /usr/include/c++/4.8/bits/cxxabi_forced.h \
 /usr/include/c++/4.8/bits/stl_function.h \
 /usr/include/c++/4.8/backward/binders.h \
 /usr/include/c++/4.8/bits/range_access.h \
 /usr/include/c++/4.8/bits/basic_string.h \
 /usr/include/c++/4.8/ext/atomicity.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/gthr.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/gthr-default.h \
 /usr/include/pthread.h /usr/include/sched.h /usr/include/time.h \
 /usr/include/x86_64-linux-gnu/bits/sched.h \
 /usr/include/x86_64-linux-gnu/bits/time.h \
 /usr/include/x86_64-linux-gnu/bits/timex.h \
 /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
 /usr/include/x86_64-linux-gnu/bits/setjmp.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/atomic_word.h \
 /usr/include/c++/4.8/bits/basic_string.tcc /usr/include/stdlib.h \
 /usr/include/x86_64-linux-gnu/bits/waitflags.h \
 /usr/include/x86_64-linux-gnu/bits/waitstatus.h \
 /usr/include/x86_64-linux-gnu/sys/types.h \
 /usr/include/x86_64-linux-gnu/sys/select.h \
 /usr/include/x86_64-linux-gnu/bits/select.h \
 /usr/include/x86_64-linux-gnu/bits/sigset.h \
 /usr/include/x86_64-linux-gnu/sys/sysmacros.h /usr/include/alloca.h \
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h /usr/include/unistd.h \
 /usr/include/x86_64-linux-gnu/bits/posix_opt.h \
 /usr/include/x86_64-linux-gnu/bits/environments.h \
 /usr/include/x86_64-linux-gnu/bits/confname.h /usr/include/getopt.h \
 /usr/include/signal.h /usr/include/x86_64-linux-gnu/bits/signum.h \
 /usr/include/x86_64-linux-gnu/bits/siginfo.h \
 /usr/include/x86_64-linux-gnu/bits/sigaction.h \
 /usr/include/x86_64-linux-gnu/bits/sigcontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigstack.h \
 /usr/include/x86_64-linux-gnu/sys/ucontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigthread.h \
 ../../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 ../../../../../resource/csdk/stack/include/octypes.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../../resource/csdk/stack/include/oc_console_logger.h \
 ../../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h ocserver.h
ocserverslow.o: ocserverslow.cpp /usr/include/stdc-predef.h \
 /usr/include/stdio.h /usr/include/features.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/wchar.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/sys_errlist.h /usr/include/string.h \
 /usr/include/xlocale.h /usr/include/stdlib.h \
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
 /usr/include/x86_64-linux-gnu/bits/stdlib-float.h /usr/include/unistd.h \
 /usr/include/x86_64-linux-gnu/bits/posix_opt.h \
 /usr/include/x86_64-linux-gnu/bits/environments.h \
 /usr/include/x86_64-linux-gnu/bits/confname.h /usr/include/getopt.h \
 /usr/include/signal.h /usr/include/x86_64-linux-gnu/bits/signum.h \
 /usr/include/x86_64-linux-gnu/bits/siginfo.h \
 /usr/include/x86_64-linux-gnu/bits/sigaction.h \
 /usr/include/x86_64-linux-gnu/bits/sigcontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigstack.h \
 /usr/include/x86_64-linux-gnu/sys/ucontext.h \
 /usr/include/x86_64-linux-gnu/bits/sigthread.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/x86_64-linux-gnu/bits/sched.h \
 /usr/include/x86_64-linux-gnu/bits/timex.h \
 /usr/include/x86_64-linux-gnu/bits/setjmp.h \
 /usr/include/x86_64-linux-gnu/sys/time.h /usr/include/c++/4.8/list \
 /usr/include/c++/4.8/bits/stl_algobase.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++config.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/os_defines.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/cpu_defines.h \
 /usr/include/c++/4.8/bits/functexcept.h \
 /usr/include/c++/4.8/bits/exception_defines.h \
 /usr/include/c++/4.8/bits/cpp_type_traits.h \
 /usr/include/c++/4.8/ext/type_traits.h \
 /usr/include/c++/4.8/ext/numeric_traits.h \
 /usr/include/c++/4.8/bits/stl_pair.h /usr/include/c++/4.8/bits/move.h \
 /usr/include/c++/4.8/bits/concept_check.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_types.h \
 /usr/include/c++/4.8/bits/stl_iterator_base_funcs.h \
 /usr/include/c++/4.8/debug/debug.h \
 /usr/include/c++/4.8/bits/stl_iterator.h \
 /usr/include/c++/4.8/bits/allocator.h \
 /usr/include/x86_64-linux-gnu/c++/4.8/bits/c++allocator.h \
 /usr/include/c++/4.8/ext/new_allocator.h /usr/include/c++/4.8/new \
 /usr/include/c++/4.8/exception \
 /usr/include/c++/4.8/bits/atomic_lockfree_defines.h \
 /usr/include/c++/4.8/bits/memoryfwd.h \
 /usr/include/c++/4.8/bits/range_access.h \
 /usr/include/c++/4.8/bits/stl_list.h /usr/include/c++/4.8/bits/list.tcc \
 ../../../../../resource/csdk/stack/include/ocstack.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h /usr/include/stdint.h \
 /usr/include/x86_64-linux-gnu/bits/wchar.h \
 ../../../../../resource/csdk/stack/include/octypes.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdbool.h \
 ../../../../../resource/csdk/stack/include/occonfig.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/internal/occonstants.h \
 ../../../../../resource/csdk/stack/include/internal/ocmessages.h \
 ../../../../../resource/csdk/stack/include/internal/ocenums.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/ocstorage.h \
 ../../../../../resource/csdk/stack/include/ocpayloads.h \
 ../../../../../resource/csdk/stack/include/occlientcallbacks.h \
 ../../../../../resource/csdk/stack/include/ocservercallbacks.h \
 ../../../../../resource/csdk/stack/include/oic_malloc.h \
 ../../../../../resource/csdk/stack/include/oic_string.h \
 ../../../../../resource/csdk/stack/include/oclogger.h \
 ../../../../../resource/csdk/stack/include/oc_logger.h \
 ../../../../../resource/csdk/stack/include/oc_logger_types.h \
 ../../../../../resource/csdk/stack/include/oc_console_logger.h \
 ocserverslow.h ../../../../../resource/csdk/stack/include/ocpayload.h \
 /usr/include/inttypes.h \
 ../../../../../resource/csdk/stack/include/payload_logging.h
