# $Id$

SOURCES = buffer.c webstr.c str.c xalloc.c strtest.c
OBJECTS = buffer.o webstr.o str.o xalloc.o strtest.o

LIBS = -lm -lc

TARGET = strtest

#CFLAGS = -Wall -pedantic -O2 -ffast-math -pipe -march=i686 -fexpensive-optimizations
CFLAGS = -Wall -pedantic 

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LIBS) -o $@ $(OBJECTS)

clean:
	rm -f strtest *.o
