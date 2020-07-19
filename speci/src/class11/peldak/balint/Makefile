# template makefile

SHELL = /bin/sh

DISTVERSION = 2
DISTNAME = bob-$(DISTVERSION)
SRC = b0.c b1.c b2.c b3.c b4.c b5.c br6.c \
	conn.c conn.h command.c command.h parse.c parse.h \
	see_data.c see_data.h game_data.c game_data.h defs.h \
	log.c log.h
AUX = Makefile

CFLAGS = -g
LDLIBS = -lm

TRG = b0 b1 b2 b3 b4 b5 br6 

default: all

all: $(TRG)

b0: b0.o conn.o command.o log.o

b1: b1.o conn.o command.o parse.o see_data.o game_data.o log.o

b2: b2.o conn.o command.o parse.o see_data.o game_data.o log.o

b3: b3.o conn.o command.o parse.o see_data.o game_data.o log.o

b4: b4.o conn.o command.o parse.o see_data.o game_data.o log.o

b5: b5.o conn.o command.o parse.o see_data.o game_data.o log.o

br6: br6.o conn.o command.o parse.o see_data.o game_data.o log.o

clean:
	rm -f *.o $(TRG)

build: clean depend all

dist: $(SRC) $(AUX)
	mkdir $(DISTNAME)
	ln $(SRC) $(AUX) $(DISTNAME)
	tar czhf $(DISTNAME).tgz $(DISTNAME)
	rm -fR $(DISTNAME)

depend:
	cc -MM *.c >Makefile.deps

-include Makefile.deps
