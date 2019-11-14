tester: tester.c board.h board.c moves.h moves.c move_generation.h move_generation.c stack.h stack.c search.h search.c evaluation.h evaluation.c
	gcc -c tester.c
	gcc -c board.c
	gcc -c moves.c
	gcc -c move_generation.c
	gcc -c stack.c
	gcc -c search.c
	gcc -c evaluation.c
	gcc -g -o tester tester.o board.o moves.o move_generation.o stack.o search.o evaluation.o
	./tester