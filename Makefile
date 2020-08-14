plainassmake:
	gcc -Wall shop.c -o shop

install:
	gcc -Wall shop.c -o shop
	cp sl /usr/local/bin/
