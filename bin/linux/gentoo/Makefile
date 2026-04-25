# Makefile for FindFriendApp on Gentoo Linux

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -Wextra -Wpedantic -std=c99

# 包含目录
INCLUDES = -I../common_core -I../common_ui

# 链接选项
LDFLAGS = -lpthread -lssl -lcrypto -lm -lsqlite3

# 源文件
CORE_SOURCES = $(wildcard ../common_core/*.c ../common_core/**/*.c)
UI_SOURCES = $(wildcard ../common_ui/*.c ../common_ui/**/*.c)
PLATFORM_SOURCES = main.c

# 目标文件
CORE_OBJECTS = $(CORE_SOURCES:.c=.o)
UI_OBJECTS = $(UI_SOURCES:.c=.o)
PLATFORM_OBJECTS = $(PLATFORM_SOURCES:.c=.o)

# 可执行文件
TARGET = FindFriendApp

all: $(TARGET)

$(TARGET): $(CORE_OBJECTS) $(UI_OBJECTS) $(PLATFORM_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(CORE_OBJECTS) $(UI_OBJECTS) $(PLATFORM_OBJECTS) $(TARGET)

.PHONY: all clean
