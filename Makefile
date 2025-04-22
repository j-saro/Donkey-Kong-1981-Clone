BIN = game

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SOURCES = $(shell find $(SRC_DIR) -name "*.c")
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

CFLAGS = -g -I$(INC_DIR) -I/usr/include -I/usr/include/cjson $(shell pkg-config --cflags gtk+-3.0) -O2
LIBS = $(shell pkg-config --libs gtk+-3.0) -lcjson -lm

$(BIN): $(OBJECTS)
	gcc -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	gcc -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD_DIR)
