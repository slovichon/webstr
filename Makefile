SOURCES = buffer.c webstr.c
OBJECTS = buffer.o webstr.o

LIBS = -lm -lc

HAS_STRREV =
HAS_XMALLOC = 
HAS_XSTRDUP =
HAS_ITOA = 

SOURCES += strrev.c
OBJECTS += strrev.o

SOURCES += xmalloc.c
OBJECTS += xmalloc.o

SOURCES += xstrdup.c
OBJECTS += xstrdup.o

SOURCES += itoa.c
OBJECTS += itoa.o

#CFLAGS = $(HAS_STRREV) $(HAS_XMALLOC) $(HAS_XSTRDUP) $(HAS_ITOA) -Wall -pedantic -O2 -ffast-math -pipe -march=i686 -fexpensive-optimizations
CFLAGS = $(HAS_STRREV) $(HAS_XMALLOC) $(HAS_XSTRDUP) $(HAS_ITOA) -Wall -pedantic 

.c.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): all

strtest: $(OBJECTS) strtest.o
	$(CC) $(LIBS) -o $@ $(OBJECTS) strtest.o

buftest: $(OBJECTS) buftest.o
	$(CC) $(LIBS) -o $@ buffer.o buftest.o

all: strtest buftest

clean:
	rm -rf strtest buftest *.o
