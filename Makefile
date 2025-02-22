CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -ggdb


build: freelist-explicit.c 
	gcc $(CFLAGS) -o build/freelist freelist-explicit.c 
