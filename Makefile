CLJHOME = $(abspath .)
#CXX = clang++
CXX = g++
CXXFLAGS=-std=c++11 -g3 -fno-inline -O0 -pthread
#CXXFLAGS=-std=c++11 -v
#  -I/usr/include/c++/4.7.0/x86_64-unknown-linux-gnu/ -I/usr/include/c++/4.7.0/

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