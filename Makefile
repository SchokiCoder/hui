C_FILES=hui.c common.c color.c hstring.c sequences.c
H_FILES=menu.h common.h color.h hstring.h sequences.h
VERSION=1.2.0
DEFINES=-D _DEFAULT_SOURCE -D _BSD_SOURCE -D _POSIX_C_SOURCE=200809L

include config.mk

hui: $(H_FILES) $(C_FILES)
	$(CC) $(COPTS) -Os -o $@ $(C_FILES) -I cfg \
		-D VERSION=\"$(VERSION)\" $(DEFINES)

d_hui: $(H_FILES) $(C_FILES)
	$(D_CC) $(COPTS) -g -o $@ $(C_FILES) -I cfg_example \
		-D VERSION=\"$(VERSION)-DEBUG\" $(DEFINES) \
		-Wall -Wextra -Wvla
		# no vla's (bigger binary and unnecessary)

clean:
	rm -f hui
	rm -f d_hui
	rm -f *.o

install: hui
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f hui $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/hui
	
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < docs/hui.1 > $(DESTDIR)$(MANPREFIX)/man1/hui.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/hui.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/hui\
		$(DESTDIR)$(MANPREFIX)/man1/hui.1

