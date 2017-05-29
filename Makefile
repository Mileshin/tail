#gcc -o main -std=c99 -pedantic -Wall -Werror main.c
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
SOURCES=main.c readbytes.c readlines.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tail

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o  $@

.c.o: 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o main
