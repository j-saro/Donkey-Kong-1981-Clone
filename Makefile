BIN = game

SRC_DIR = src
INC_DIR = include

SOURCES = $(shell find $(SRC_DIR) -name "*.c")
OBJ = $(SOURCES:.c=.o)

CFLAGS = -g -I$(INC_DIR) -I/usr/include -I/usr/include/cjson $(shell pkg-config --cflags gtk+-3.0) -O2

LIBS = $(shell pkg-config --libs gtk+-3.0) -lcjson -lm

$(BIN): $(OBJ)
	gcc -o $@ $(OBJ) $(CFLAGS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) $(OBJ)
