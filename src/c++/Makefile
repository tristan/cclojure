CLJHOME=$(abspath ../..)
#CXX=clang++
CXX=g++
CXXFLAGS=-std=c++11
CC=$(CXX)
CFLAGS=$(CXXFLAGS)

EXENAME=cclj
SRCS = repl lispreader object cclj
LIBS = $(SRCS:%=%.o)

default: $(EXENAME)
	mv cclj $(CLJHOME)/$(EXENAME)

$(EXENAME): $(LIBS)


clean:
	rm -f *~ *#
	rm -f *.o
	rm -f $(CLJHOME)/$(EXENAME)