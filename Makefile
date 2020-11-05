normal:
	tcc -Wextra -g list.c -o list

install:
	tcc -Wextra -g list.c -o list
	cp list /usr/local/bin/

clean:
	rm ./list
