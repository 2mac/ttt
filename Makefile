EXEC=ttt
EXECCFLAGS=-Wall -std=c11
SOURCES=ttt.c
prefix=/usr/local

all: ttt

ttt:
	$(CC) $(EXECCFLAGS) $(CFLAGS) -o $(EXEC) $(SOURCES)

install: ttt
	install -m755 $(EXEC) $(DESTDIR)$(prefix)/bin/$(EXEC)

clean:
	rm -f ttt
