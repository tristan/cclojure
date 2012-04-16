CLJHOME=$(abspath ../..)
#CC = clang -v NOTE: clang doesn't seem to support function alignmnet, meaning some things break
CC = gcc -std=c99

NAME = clojure

SRCS = $(NAME).c ios.c utf8.c timefuncs.c types.c repl.c lispreader.c
OBJS = $(SRCS:%.c=%.o)
DOBJS = $(SRCS:%.c=%.do)
EXENAME = cclj
LIBTARGET = lib$(NAME)

# OS flags: LINUX, WIN32, MACOSX
# architecture flags: __CPU__=xxx, BITS64, ARCH_X86, ARCH_X86_64
CONFIG = -DLINUX -DARCH_X86_64 -DBITS64 -D__CPU__=686
FLAGS = -falign-functions -Wall -Wno-strict-aliasing $(CFLAGS) -DUSE_COMPUTED_GOTO $(CONFIG)
LIBFILES = $(LLT)
LIBS = $(LIBFILES) -lm

DEBUGFLAGS = -g -DDEBUG $(FLAGS)
SHIPFLAGS = -O2 -DNDEBUG $(FLAGS)

default: debug

#test:
#cd $(CLJHOME)/tests && ../$(EXENAME) unittest.lsp

%.o: %.c
	$(CC) $(SHIPFLAGS) -c $< -o $@
%.do: %.c
	$(CC) $(DEBUGFLAGS) -c $< -o $@

clojure.o: clojure.c repl.c clojure.h
clojure.do: clojure.c repl.c clojure.h
cclj.o: cclj.c clojure.h
cclj.do: cclj.c clojure.h
#playground.o: playground.c clojure.h

$(LIBTARGET).da: $(DOBJS)
	rm -rf $@
	ar rs $@ $(DOBJS)

$(LIBTARGET).a: $(OBJS)
	rm -rf $@
	ar rs $@ $(OBJS)

debug: $(DOBJS) $(LIBFILES) $(LIBTARGET).da cclj.do
	$(CC) $(DEBUGFLAGS) $(DOBJS) cclj.do -o $(CLJHOME)/$(EXENAME) $(LIBS) $(LIBTARGET).da
# make test

release: $(OBJS) $(LIBFILES) $(LIBTARGET).a cclj.o
	$(CC) $(SHIPFLAGS) $(OBJS) cclj.o -o $(CLJHOME)/$(EXENAME) $(LIBS) $(LIBTARGET).a

#playground: $(DOBJS) $(LIBFILES) $(LIBTARGET).a playground.o
#$(CC) $(DEBUGFLAGS) $(DOBJS) playground.o -o $(CLJHOME)/playground $(LIBS) $(LIBTARGET).a

clean:
	rm -f *~ *#
	rm -f *.o
	rm -f *.do
	rm -f $(CLJHOME)/$(EXENAME)
	rm -f $(CLJHOME)/playground
	rm -f $(LIBTARGET).a
	rm -f $(LIBTARGET).da