CC=$(shell mtev-config --cc)
CFLAGS=$(shell mtev-config --cflags) -g -Wall
CPPFLAGS=$(shell mtev-config --cppflags)
LDFLAGS=$(shell mtev-config --ldflags)

all: fql

fql: fql.c
	$(CC) $< $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ -lmtev -ldl
