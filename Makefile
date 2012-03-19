CFLAGS=-Wall -g -std=c99 -licuuc -licudata -licui18n -licuio -lgmp

LISTS=src/c/list.o src/c/arraylist.o
NUMBERS=src/c/number.o src/c/integer.o src/c/decimal.o src/c/ratio.o
READERS=src/c/reader.o src/c/file_reader.o src/c/line_number_reader.o
OBJECTS=src/c/object.o src/c/string.o ${NUMBERS} ${READERS} ${LISTS}

src/c/main: ${OBJECTS} src/c/lisp_reader.o src/c/string_buffer.o src/c/unicode_utils.o src/c/regex_icu.o

src/c/whiteboard: src/c/object.o src/c/string.o

cclj: src/c/main
	mkdir -p bin
	mv src/c/main bin/cclj

src/c/testing: src/c/object.o

testing: src/c/testing
	mkdir -p bin
	mv src/c/testing bin/testing

whiteboard: src/c/whiteboard
	mkdir -p bin
	mv src/c/whiteboard bin/

all: cclj

clean:
	find . -iname '*.o' -exec rm {} \;
	rm bin/*

clean-emacs:
	find . -iname '*~' -exec rm {} \;