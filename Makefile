CC=$(shell mtev-config --cc)
CFLAGS=$(shell mtev-config --cflags) -g -Wall
CPPFLAGS=$(shell mtev-config --cppflags)
LDFLAGS=$(shell mtev-config --ldflags)
PREFIX = /usr/local

all: fql

fql: fql.c
	$(CC) $< $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ -lmtev -ldl


.PHONY: install
install: fql
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 fql $(DESTDIR)$(PREFIX)/bin

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/fql
