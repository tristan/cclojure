CFLAGS=-Wall -g -std=c99 -licuuc -licudata -licui18n -licuio -lgmp

src/c/main: src/c/unicode_utils.o src/c/numbers_gmp.o src/c/regex_icu.o src/c/string_buffer.o src/c/lisp_reader.o

cclj: src/c/main
	mkdir -p bin
	mv src/c/main bin/cclj

src/c/testing: src/c/unicode_utils.o src/c/numbers_gmp.o src/c/regex_icu.o

testing: src/c/testing
	mkdir -p bin
	mv src/c/testing bin/testing

all: cclj

clean:
	find . -iname '*.o' -exec rm {} \;
	rm bin/*

clean-emacs:
	find . -iname '*~' -exec rm {} \;