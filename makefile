TARGET = handling
LIBS = -lm
CC = gcc
CFLAGS = -ansi -Wall -Werror

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
    $(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
    $(CC) $(OBJECTS) -ansi -Wall -Werror $(LIBS) -o $@

clean:
    -rm -f *.o
    -rm -f $(TARGET)