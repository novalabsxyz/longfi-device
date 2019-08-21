longfi : longfi.o sx126x.o sx1276.o

SRCDIR=./

CC=gcc
CFLAGS=-I$(SRCDIR)


longfi.o : longfi.c longfi.h
	$(CC) -c longfi.c $(CFLAGS)
sx126x.o : radio/sx126x/sx126x.c radio/sx126x/sx126x-board.c radio/sx126x/radio.c board.c board.h
	$(CC) -c radio/sx126x/sx126x.c $(CFLAGS)
# sx1276.o : radio/sx126x/sx126x.c radio/sx126x/sx126x-board.c radio/sx126x/radio.c board.c
#         cc -c command.c

clean :
	rm edit main.o kbd.o command.o display.o \
	insert.o search.o files.o utils.o