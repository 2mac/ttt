CC=c99
CFLAGS=-Wall -Wextra -Wunreachable-code -ftrapv -fPIC -D_POSIX_C_SOURCE=2
PREFIX=/usr/local

all: ttt

ttt_deps=ttt.o
ttt: $(ttt_deps)
	$(CC) -o $@ $(ttt_deps)

install: ttt
	install -m755 $(EXEC) $(DESTDIR)$(PREFIX)/bin/$(EXEC)

clean:
	rm -f *.o ttt
