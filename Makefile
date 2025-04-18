BIN = game

SRC_DIR = src
INC_DIR = include

SOURCES = $(shell find $(SRC_DIR) -name "*.c")
OBJ = $(SOURCES:.c=.o)

CFLAGS = -I$(INC_DIR) $(shell pkg-config --cflags gtk+-3.0) -O4
LIBS = $(shell pkg-config --libs gtk+-3.0)

$(BIN): $(OBJ)
	gcc -o $@ $(OBJ) $(CFLAGS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) $(OBJ)
