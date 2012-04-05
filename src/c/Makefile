CLJHOME=$(abspath ../..)
CC = clang

NAME = clojure
SRCS = $(NAME).c builtins.c string.c equalhash.c table.c iostream.c
OBJS = $(SRCS:%.c=%.o)
DOBJS = $(SRCS:%.c=%.do)
EXENAME = cclj
LIBTARGET = lib$(NAME)
LLTDIR = llt
LLT = $(LLTDIR)/libllt.a

# OS flags: LINUX, WIN32, MACOSX
# architecture flags: __CPU__=xxx, BITS64, ARCH_X86, ARCH_X86_64
CONFIG = -DLINUX -DARCH_X86_64 -DBITS64 -D__CPU__=686
FLAGS = -falign-functions -Wall -Wno-strict-aliasing -I$(LLTDIR) $(CFLAGS) -DUSE_COMPUTED_GOTO $(CONFIG)
LIBFILES = $(LLT)
LIBS = $(LIBFILES) -lm

DEBUGFLAGS = -g -DDEBUG $(FLAGS)
SHIPFLAGS = -O2 -DNDEBUG $(FLAGS)

default: debug playground

test:
	cd $(CLJHOME)/tests && ../$(EXENAME) unittest.lsp

%.o: %.c
	$(CC) $(SHIPFLAGS) -c $< -o $@
%.do: %.c
	$(CC) $(DEBUGFLAGS) -c $< -o $@

clojure.o:  clojure.c cvalues.c operators.c types.c clojure.h print.c read.c equal.c
clojure.do: clojure.c cvalues.c operators.c types.c clojure.h print.c read.c equal.c
cljmain.o: cljmain.c clojure.h
cljmain.do: cljmain.c clojure.h
playground.o: playground.c clojure.h

$(LLT):
	cd $(LLTDIR) && make

$(LIBTARGET).da: $(DOBJS)
	rm -rf $@
	ar rs $@ $(DOBJS)

$(LIBTARGET).a: $(OBJS)
	rm -rf $@
	ar rs $@ $(OBJS)

debug: $(DOBJS) $(LIBFILES) $(LIBTARGET).da cljmain.do
	$(CC) $(DEBUGFLAGS) $(DOBJS) cljmain.do -o $(CLJHOME)/$(EXENAME) $(LIBS) $(LIBTARGET).da
#	make test

release: $(OBJS) $(LIBFILES) $(LIBTARGET).a cljmain.o
	$(CC) $(SHIPFLAGS) $(OBJS) cljmain.o -o $(CLJHOME)/$(EXENAME) $(LIBS) $(LIBTARGET).a

playground:	$(DOBJS) $(LIBFILES) $(LIBTARGET).a playground.o
	$(CC) $(DEBUGFLAGS) $(DOBJS) playground.o -o $(CLJHOME)/playground $(LIBS) $(LIBTARGET).a

clean:
	rm -f *~ *#
	rm -f *.o
	rm -f *.do
	rm -f $(CLJHOME)/$(EXENAME)
	rm -f $(CLJHOME)/playground
	rm -f $(LIBTARGET).a
	rm -f $(LIBTARGET).da
