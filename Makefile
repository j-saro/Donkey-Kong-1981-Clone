BIN = game

SRC_DIR = src
INC_DIR = include
LIB_DIR = lib/cjson
BUILD_DIR = build

SOURCES = $(shell find $(SRC_DIR) -name "*.c") $(LIB_DIR)/cJSON.c
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

CFLAGS = -g -I$(INC_DIR) -I$(LIB_DIR) $(shell pkg-config --cflags gtk+-3.0 cairo) -O2

LIBS = $(shell pkg-config --libs gtk+-3.0 cairo) -lm

$(BIN): $(OBJECTS)
	gcc -o $@ $^ $(LIBS)

# Compile .c -> .o into build/
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD_DIR)
