plainassmake:
	clang -Wextra -g shop.c -o shop

install:
	clang -Wextra -g shop.c -o shop
	cp sl /usr/local/bin/

clean:
	rm ./shop
