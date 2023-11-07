SHARED_CFILES=common.c color.c hstring.c sequences.c
SHARED_HFILES=common.h color.h hstring.h menu.h sequences.h
VERSION=1.2.0
DEFINES=-D _DEFAULT_SOURCE -D _BSD_SOURCE -D _POSIX_C_SOURCE=200809L

# no vla's (bigger binary and unnecessary)
D_COPTS=-Wall -Wextra -Wvla -Wno-unused-variable -fsanitize=address,undefined
TEST_COPTS=-Wall -Wextra -Wvla -I .

include config.mk

hui: hui.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(CC) $(COPTS) -Os -o $@ $< $(SHARED_CFILES) -I cfg \
		-D VERSION=\"$(VERSION)\" $(DEFINES) \
		-D CONFIG_HUI

d_hui: hui.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(D_CC) $(COPTS) -g -o $@ $< $(SHARED_CFILES) -I cfg_example \
		-D VERSION=\"$(VERSION)-DEBUG\" $(DEFINES) $(D_COPTS) \
		-D CONFIG_HUI

courier: courier.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(CC) $(COPTS) -Os -o $@ $< $(SHARED_CFILES) -I cfg \
		-D VERSION=\"$(VERSION)\" $(DEFINES) \
		-D CONFIG_COURIER

d_courier: courier.c $(SHARED_CFILES) $(SHARED_HFILES)
	$(D_CC) $(COPTS) -g -o $@ $< $(SHARED_CFILES) -I cfg_example \
		-D VERSION=\"$(VERSION)-DEBUG\" $(DEFINES) $(D_COPTS) \
		-D CONFIG_COURIER

t_hstring: t_hstring.c hstring.c color.c
	$(CC) $(COPTS) -g -o $@ $^ $(TEST_COPTS)

clean:
	rm -f hui d_hui courier d_courier t_hstring

hui.1:
	sed "s/VERSION/$(VERSION)/g" < docs/hui.1 > hui.1
