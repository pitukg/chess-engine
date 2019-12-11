tester: tester.c board.h board.c moves.h moves.c move_generation.h move_generation.c stack.h stack.c search.h search.c evaluation.h evaluation.c transposition.c
	gcc -c tester.c
	gcc -c -O3 board.c
	gcc -c -O3 moves.c
	gcc -c -O3 move_generation.c
	gcc -c -O3 stack.c
	gcc -c -O3 search.c
	gcc -c -O3 evaluation.c
	gcc -c -O3 transposition.c
	gcc -g -o tester tester.o board.o moves.o move_generation.o stack.o search.o evaluation.o transposition.o
	./tester

build:
	gcc -c tester.c
	gcc -c -O3 board.c
	gcc -c -O3 moves.c
	gcc -c -O3 move_generation.c
	gcc -c -O3 stack.c
	gcc -c -O3 search.c
	gcc -c -O3 evaluation.c
	gcc -c -O3 transposition.c
	gcc -g -o tester tester.o board.o moves.o move_generation.o stack.o search.o evaluation.o transposition.o

run:
	./tester

clean:
	rm tester tester.o board.o moves.o move_generation.o stack.o search.o evaluation.o transposition.o