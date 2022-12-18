CC = cc
COPTS = -std=c99 -pedantic

hui: clean
	$(CC) ${COPTS} -Os -o $@ src/*.c

debug: clean
	$(CC) ${COPTS} -g -o hui src/*.c -Wall -Wextra

clean:
	rm -f hui
