CFLAGS=-Wall -g -std=c99 -lonig

src/c/main: src/c/string_buffer.o src/c/lisp_reader.o

cclj: src/c/main
	cp src/c/main bin/cclj

all: cclj