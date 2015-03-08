CC      = gcc -std=c11 -Wall -g

.PHONY: all clean test
all: bin build bin/test

bin:
	mkdir -p bin

build:
	mkdir -p build

bin/test: build/test.o build/scope.o
	$(CC) -o bin/test build/test.o build/scope.o -lc

build/test.o: src/test.c src/scope.h
	$(CC) -c src/test.c -o build/test.o
build/scope.o: src/scope.c src/scope.h
	$(CC) -c src/scope.c -o build/scope.o

clean:
	rm -r bin
	rm -r build

test: bin/test
	./bin/test
