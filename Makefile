BIN = game

SOURCES = $(shell find . -name "*.c")
OBJ = $(SOURCES:.c=.o)
HEADERS = $(shell find . -name "*.h")

CFLAGS = $(shell pkg-config --cflags gtk+-3.0) -O4
LIBS = $(shell pkg-config --libs gtk+-3.0)

$(BIN): $(OBJ)
	gcc -o $@ $(CFLAGS) $^ $(LIBS)

%.o: %.c $(HEADERS)
	gcc -c -o $@ $(CFLAGS) $< $(LIBS)

clean:
	rm -f $(BIN) $(OBJ)
