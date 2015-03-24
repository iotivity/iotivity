CXX = g++

CXX_FLAGS = -std=c++0x -Wall
LIB = ../../../lib
CPLUFF_DIR = $(LIB)/cpluff/libcpluff

SRC = ../../src/

CXX_INC    = -I. -I$(CPLUFF_DIR)
CXX_INC += -I$(LIB)/rapidxml 

LINK_LIB = -lboost_system -ldl -lexpat -lboost_thread 
CXX_LIBS = $(CPLUFF_DIR)/.libs/libcpluff.a 

PHONY :libppm.a libpmimpl.so

all: .PHONY
	
libppm.a:PluginManager.o  Plugin.o 
	ar -cvr libppm.a  PluginManager.o  Plugin.o 

PluginManager.o : $(SRC)PluginManager.cpp
	$(CXX) $(CXX_INC) -c $(CXX_FLAGS) -Wsign-compare $(SRC)PluginManager.cpp 


libpmimpl.so:PluginManagerImpl.o Plugin.o CpluffAdapter.o FelixAdapter.o Config.o $(CPLUFF_DIR)/.libs/libcpluff.a 
	$(CXX) -shared -o libpmimpl.so PluginManagerImpl.o Plugin.o CpluffAdapter.o FelixAdapter.o Config.o $(CXX_LIBS) $(LINK_LIB)  

PluginManagerImpl.o : $(SRC)PluginManagerImpl.cpp
	$(CXX) $(CXX_INC)  -fPIC  -c $(CXX_FLAGS) -Wsign-compare $(SRC)PluginManagerImpl.cpp

Plugin.o : $(SRC)Plugin.cpp
	$(CXX) $(CXX_INC)  -fPIC  -c $(SRC)Plugin.cpp

Config.o : $(SRC)Config.cpp
	$(CXX) $(CXX_INC)  -fPIC  -c $(SRC)Config.cpp

CpluffAdapter.o  : $(SRC)CpluffAdapter.cpp
	$(CXX) $(CXX_INC) -c $(CXX_FLAGS) -Wsign-compare $(SRC)CpluffAdapter.cpp

FelixAdapter.o : $(SRC)FelixAdapter.cpp
	$(CXX) $(CXX_INC) -c $(CXX_FLAGS) -Wsign-compare $(SRC)FelixAdapter.cpp

clean:	
	rm -rf *.o
	rm -rf *.a
	rm -rf *.so
