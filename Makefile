SRC = hui.c color.c hstring.c sequences.c
VERSION = 1.1.1
DEFINES = -D _DEFAULT_SOURCE -D _BSD_SOURCE -D _POSIX_C_SOURCE=200809L

include config.mk

hui: clean
	$(CC) ${COPTS} -Os -o $@ ${SRC} -D VERSION=\"${VERSION}\" ${DEFINES}

debug: clean
	$(CC) ${COPTS} -g -o hui ${SRC} -Wall -Wextra -Wimplicit-fallthrough=0 \
	-D VERSION=\"${VERSION}-DEBUG\" ${DEFINES}

clean:
	rm -f hui

install: hui
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hui ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hui
	
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < docs/hui.1 > ${DESTDIR}${MANPREFIX}/man1/hui.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/hui.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/hui\
		${DESTDIR}${MANPREFIX}/man1/hui.1
