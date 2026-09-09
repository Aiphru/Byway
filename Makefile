all:
	gcc main.c serv.c parser.c -o a.out && ./a.out
testing:
	gcc -fsanitize=address,undefined *.c -o a.out && ./a.out 
