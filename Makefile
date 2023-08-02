all: ./build sh

./build:
	mkdir build

sh: command.o parse.o sh.o
	ld -o ./build/sh ./build/command.o ./build/parse.o ./build/sh.o

command.o: ./src/command.c
	gcc -O1 -g3 -ggdb -ffreestanding -c ./src/command.c -o ./build/command.o

parse.o: ./src/parse.c
	gcc -O1 -g3 -ggdb -ffreestanding -c ./src/parse.c -o ./build/parse.o

sh.o: ./src/sh.c
	gcc -O1 -g3 -ggdb -ffreestanding -c ./src/sh.c -o ./build/sh.o

clean:
	rm -f sh *.o