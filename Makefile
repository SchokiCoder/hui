SHARED_CFILES=common.c color.c hstring.c sequences.c
SHARED_HFILES=common.h color.h hstring.h menu.h sequences.h
VERSION=1.2.0
DEFINES=-D _DEFAULT_SOURCE -D _BSD_SOURCE -D _POSIX_C_SOURCE=200809L
D_COPTS=-Wall -Wextra -Wvla -Wno-unused -fsanitize=address,undefined

include config.mk

hui: hui.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(CC) $(COPTS) -Os -o $@ $< $(SHARED_CFILES) -I cfg \
		-D VERSION=\"$(VERSION)\" $(DEFINES)

# no vla's (bigger binary and unnecessary)
d_hui: hui.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(D_CC) $(COPTS) -g -o $@ $< $(SHARED_CFILES) -I cfg_example \
		-D VERSION=\"$(VERSION)-DEBUG\" $(DEFINES) $(D_COPTS)

courier: courier.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(CC) $(COPTS) -Os -o $@ $< $(SHARED_CFILES) -I cfg \
		-D VERSION=\"$(VERSION)\" $(DEFINES)

d_courier: courier.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(CC) $(COPTS) -Os -o $@ $< $(SHARED_CFILES) -I cfg_example \
		-D VERSION=\"$(VERSION)\" $(DEFINES) $(D_COPTS)

clean:
	rm -f hui
	rm -f d_hui
	rm -f courier
	rm -f d_courier
	rm -f *.o

install: hui courier
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f hui $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/hui
	
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < docs/hui.1 > $(DESTDIR)$(MANPREFIX)/man1/hui.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/hui.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/hui\
		$(DESTDIR)$(MANPREFIX)/man1/hui.1
