# $Id$

OBJECTS = webstr.o str.o xalloc.o
LIBS = -lm -lc -L../buffer -lbuffer -L. -lresolv
CFLAGS = -Wall -I../buffer
TARGET = libwebstr.so

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -shared $(LIBS) -o $@ $(OBJECTS)
	chmod 444 $@

test: $(TARGET) strtest.o
	$(CC) $(LIBS) -o strtest -lbuffer -lwebstr strtest.o

clean:
	rm -f $(TARGET) $(OBJECTS) strtest.o strtest
