CLJHOME = $(abspath .)

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