#
#
#

CC=gcc
CFLAGS=-g -O4 -pipe
LIBS=-lsocket -lnsl

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
