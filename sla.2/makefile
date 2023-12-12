all: prog.exe

prog.exe: fc.o main.o
	gcc -o $@ $^

fc.o: fc.c
	gcc -o $@ -c $<

main.o: main.c 
	gcc -o $@ -c $<
