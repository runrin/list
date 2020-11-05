include config.mk

normal:
	${CC} ${CFLAGS} list.c -o list

install: normal
	cp list ${DIR}

clean:
	rm list

uninstall:
	rm ${DIR}/list
