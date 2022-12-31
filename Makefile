CC = cc
COPTS = -std=c99 -pedantic
SRC = hui.c
LIBS = -l termbox

hui: clean
	$(CC) ${COPTS} ${LIBS} ${LOPTS} -Os -o $@ ${SRC}

debug: clean
	$(CC) ${COPTS} ${LIBS} ${LOPTS} -g -o hui ${SRC} -Wall -Wextra

clean:
	rm -f hui

