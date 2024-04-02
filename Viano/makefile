finish:
	gcc -Wall -Werror -o a.out *.c -lm 
	./main.out 8 19 0.01 4096 4 0.75 32 > test.txt

main:
	# gcc -Wall -Werror -D DEBUG_MODE -g main.c -o main.out -lm
	# Guide for output: Usage: ./main <n> <seed> <lambda> <upper-bound> <time for context switch> <alpha> <time slice>
	gcc -Wall -Werror -o a.out *.c -lm 
	./main.out 8 19 0.01 4096 4 0.75 32 > test.txt
	# valgrind --tool=memcheck --leak-check=full --show-reachable=yes --num-callers=20 --track-fds=yes ./main.out 8 19 0.01 4096 4 0.75 32 > test.txt


test2:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 1 19 0.01 4096 4 0.5 64 > test02.txt
test3:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 2 19 0.01 4096 4 0.5 64 > test03.txt
test4:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 8 19 0.01 4096 4 0.75 32 > test04.txt
test5:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 8 101 0.001 16384 4 0.5 128 > test05.txt

test6:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 19 19 0.001 16384 4 0.5 128 > test06.txt

test7:
	gcc -Wall -Werror -o a.out *.c -lm 
	./a.out 26 19 0.001 32384 4 0.1 16 > test07.txt