include config.mk

normal:
	${CC} ${CFLAGS} list.c -o list

install: normal
	cp list ${PREFIX}/bin

clean:
	rm list

uninstall:
	rm ${PREFIX}/bin/list
