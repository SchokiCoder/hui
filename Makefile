CC = cc
COPTS = -std=c99 -pedantic
SRC = hui.c

hui: clean
	$(CC) ${COPTS} -Os -o $@ ${SRC}

debug: clean
	$(CC) ${COPTS} -g -o hui ${SRC} -Wall -Wextra

clean:
	rm -f hui

