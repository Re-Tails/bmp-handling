LIBS = -lm
CC = gcc
CFLAGS = -ansi -Wall -Werror

HEADERS = $(wildcard *.h)

handling.out: handling.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ handling.c $(LIBS)

clean:
	-rm -f *.out