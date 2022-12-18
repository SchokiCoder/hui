CC = cc
COPTS = -std=c99 -pedantic

hui: clean
	$(CC) ${COPTS} -Os -o $@ src/*.c

clean:
	rm -f hui
