BIN = game

SRC_DIR = src
INC_DIR = include
LIB_DIR = lib/cjson
BUILD_DIR = build

# Include cJSON.c in the compilation
SOURCES = $(shell find $(SRC_DIR) -name "*.c") $(LIB_DIR)/cJSON.c
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Add lib/cjson to the header include path
CFLAGS = -g -I$(INC_DIR) -I$(LIB_DIR) $(shell pkg-config --cflags gtk+-3.0) -O2

# Remove system -lcjson since we compile our own
LIBS = $(shell pkg-config --libs gtk+-3.0) -lm

$(BIN): $(OBJECTS)
	gcc -o $@ $^ $(LIBS)

# Compile .c -> .o into build/
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD_DIR)
