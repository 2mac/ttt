EXEC=ttt
CFLAGS=-Wall -std=c11
SOURCES=ttt.c
prefix=/usr/local

all: executable

executable:
	$(CC) $(CFLAGS) -o $(EXEC) $(SOURCES)

install: executable
	install -m755 $(EXEC) $(DESTDIR)$(prefix)/bin/$(EXEC)

clean:
	rm -f ttt
