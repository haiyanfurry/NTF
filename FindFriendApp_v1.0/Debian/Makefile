CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 webkit2gtk-4.1`
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.1` -lpthread -lm -lsqlite3

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(shell find $(SRC_DIR) -name "*.c" 2>/dev/null)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = FindFriendApp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)
	@echo "✅ 编译成功: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

install-deps:
	sudo apt install -y libgtk-3-dev libwebkit2gtk-4.1-dev

.PHONY: all clean run install-deps
