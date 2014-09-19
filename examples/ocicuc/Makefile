OCLIB=../..
OCLIB_LIB=../../$(BUILD)/obj/liboc.a #$(OCLIB)/release/obj/liboc.a

BOOST_BASE=/usr/local/boost
BOOST_INC=$(BOOST_BASE)/include
BOOST_LIB=$(BOOST_BASE)/lib

CXX_FLAGS.debug     := -g3 -O0
CXX_FLAGS.release   := -O3
CXX_FLAGS=-Wall -std=c++0x -ggdb $(CXX_FLAGS.$(BUILD))

# There's probably nicer Makefile magic for this, but hopefully it will suffice:
CXX_INC=-I$(OCLIB)/include \
	-I$(BOOST_INC) \
	-I../../include/ \
	-I../../csdk/stack/include \
	-I../../csdk/ocsocket/include \
	-I../../csdk/ocrandom/include \
	-I../../csdk/logger/include

CXX_LIBS=-L$(BOOST_LIB) $(OCLIB_LIB) ../../csdk/release/liboctbstack.a -lpthread -lboost_program_options

.PHONY: client server

all: client server monoprocess
	@echo Remember to \"export LD_LIBRARY_PATH=$(BOOST_LIB)\:\$$LD_LIBRARY_PATH\"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CXX_FLAGS) $(CXX_INC) -c -o $@ $<

client: client.o driver.o utility.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

server: server.o driver.o utility.o light_resource.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

monoprocess: monoprocess.o driver.o utility.o light_resource.o
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(CXX_LIBS)

clean:
	rm -f *.o server client monoprocess
