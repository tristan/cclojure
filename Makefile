CLJHOME = $(abspath .)

#all: cclj

release:
	cd src/c && make

clean:
	rm -f *~ *#
	cd src/c && make clean

test: cclj
	cd test && make