SRC = hui.c color.c hstring.c sequences.c
VERSION = 0.3.0

include config.mk

hui: clean
	$(CC) ${COPTS} -Os -o $@ ${SRC} -D VERSION=\"${VERSION}\"

debug: clean
	$(CC) ${COPTS} -g -o hui ${SRC} -Wall -Wextra -D VERSION=\"${VERSION}-DEBUG\"

clean:
	rm -f hui

install: hui
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hui ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hui
	
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < hui.1 > ${DESTDIR}${MANPREFIX}/man1/hui.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/hui.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/hui\
		${DESTDIR}${MANPREFIX}/man1/hui.1
