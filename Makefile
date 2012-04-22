CLJHOME = $(abspath .)
CXX = clang++
CXXFLAGS=-std=c++11 -g3 -fno-inline -O0

default: c++

c:
	cd src/c && make

c++:
	cd src/c++ && make

clean:
	rm -f *~ *#
	cd src/c && make clean
	cd src/c++ && make clean

test: cclj
	cd test && make

whiteboard: test/c++/whiteboard
	./test/c++/whiteboard