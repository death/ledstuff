CC=gcc
CFLAGS=-Wall
LIBS=-lm -lcurses
DEPS=common led
TARGETS=evenodd travel morse

%.o: %.c $(patsubst %, %.h, $(DEPS))
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

$(TARGETS): %: %.o $(patsubst %, %.o, $(DEPS))
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o
