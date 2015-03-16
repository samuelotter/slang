TARGET   = bin/test
CC       = gcc
CC_FLAGS = -std=c11 -Wall -Werror -g -D_DEFAULT_SOURCE

HEADER_FILES = $(wildcard src/*.h)
SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES = $(SOURCE_FILES:src/%.c=build/%.o)

.PHONY: all clean test
all: bin build $(TARGET)

bin:
	mkdir -p bin

build:
	mkdir -p build

$(TARGET): $(OBJECT_FILES)
	$(CC) -o bin/test $(OBJECT_FILES) -lc

build/%.o: src/%.c $(HEADER_FILES)
	$(CC) -c $< -o $@ $(CC_FLAGS)

clean:
	rm -r bin
	rm -r build

test: $(TARGET)
	valgrind ./bin/test

check-syntax:
	$(CC) -o nul -S ${SOURCE_FILES} ${HEADER_FILES}
