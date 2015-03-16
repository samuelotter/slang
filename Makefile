TARGET   = bin/test

HEADER_FILES = $(wildcard include/slang/*.h)
SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES = $(SOURCE_FILES:src/%.c=build/%.o)
TEST_FILES   = $(wildcard tests/*.c)
TEST_TARGETS = $(TEST_FILES:tests/%.c=build/tests/%)

CC       = gcc
CC_FLAGS = -std=c11 -Wall -Werror -g -D_DEFAULT_SOURCE -I include/slang

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

build/tests/%: tests/%.c
	mkdir -p build/tests
	$(CC) $< -o $@ $(CC_FLAGS) $(OBJECT_FILES)

define run-test =
	@valgrind -q $(1) > $(1).log 2>&1; \
	if [ $$? == 0 ]; then echo "OK   " $(1);\
	else echo "FAIL " $(1); cat $(1).log | sed "s/^/>    /"; fi;
	@printf "\n"
endef

test: $(TEST_TARGETS)
	@printf "== Running tests ====\n"
	$(foreach t, $(TEST_TARGETS), $(call run-test, $(t)))

check-syntax:
	$(CC) -o nul -S ${SOURCE_FILES} ${HEADER_FILES}
