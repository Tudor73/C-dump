CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -ggdb


build: freelist-implicit.c 
	gcc $(CFLAGS) -o build/freelist freelist-implicit.c 
