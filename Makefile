plainassmake:
	gcc -Wall shop.c -o sl

install:
	gcc -Wall shop.c -o sl
	cp sl /usr/local/bin/
