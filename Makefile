.PHONY: all
all:snake.exe 
snake.exe: event.o main.o terminal.o 
	gcc -std=c99 -O3  -o snake.exe main.o terminal.o event.o 
main.o: main.c
	gcc -std=c99 -O3 -Wall -o main.o -g -c main.c
terminal.o: terminal.c
	gcc -std=c99 -O3 -Wall -o terminal.o -c terminal.c
event.o: event.c
	gcc -std=c99 -O3 -Wall -o event.o -c event.c
clean:
	rm -rf *.o
	gcc -std=c99 -O3 -Wall -o main.o -g -c main.c

