APP_NAME = hui
CC = cc
COPTS = -std=c99 -pedantic

debug:
	$(CC) ${COPTS} -g -o ${APP_NAME} src/*.c

release:
	$(CC) ${COPTS} -O3 -o ${APP_NAME} src/*.c

