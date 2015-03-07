CC      = gcc -std=c11 -Wall -g

.PHONY: all clean test
all: bin build bin/test

bin:
	mkdir -p bin

build:
	mkdir -p build

bin/test: build/test.o build/scope.o build/list.o
	$(CC) -o bin/test build/list.o build/test.o build/scope.o -lc

OBJ_FILES = list.o \
	    scope.o \
	    test.o

build/list.o: src/list.c src/list.h src/scope.h
	$(CC) -c src/list.c -o build/list.o
build/scope.o: src/scope.c src/scope.h
	$(CC) -c src/scope.c -o build/scope.o
build/test.o: src/test.c src/scope.h
	$(CC) -c src/test.c -o build/test.o

clean:
	rm -r bin
	rm -r build

test: bin/test
	./bin/test
