plainassmake:
	clang -Wextra -g list.c -o list

install:
	clang -Wextra -g list.c -o list
	cp list /usr/local/bin/

clean:
	rm ./list
