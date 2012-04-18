CLJHOME=$(abspath ../..)
#CXX=clang++
CXX=g++
CXXFLAGS=-std=c++11 -g3 -fno-inline -O0
CC=$(CXX)
CFLAGS=$(CXXFLAGS)

EXENAME=cclj
SRCS = object numbers utf8 lispreader repl cclj
LIBS = $(SRCS:%=%.o)

default: $(EXENAME)
	mv cclj $(CLJHOME)/$(EXENAME)

$(EXENAME): $(LIBS)

clean:
	rm -f *~ *#
	rm -f *.o
	rm -f $(CLJHOME)/$(EXENAME)