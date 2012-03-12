CFLAGS=-Wall -g -std=c99 -licuuc -licudata -licui18n

src/c/main: src/c/regex.o src/c/string_buffer.o src/c/lisp_reader.o

cclj: src/c/main
	mv src/c/main bin/cclj

src/c/testing: src/c/regex.o 

testing: src/c/testing
	mv src/c/testing bin/testing

all: cclj

clean:
	find . -iname '*.o' -exec rm {} \;
	rm bin/*

clean-emacs:
	find . -iname '*~' -exec rm {} \;