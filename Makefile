APP_NAME = house_de
CC=cargo rustc

debug:
	$(CC)

release:
	$(CC) --release

add-shell:
	printf "\n/usr/bin/${APP_NAME}\n" >> /etc/shells

set-shell:
	usermod --shell /usr/bin/${APP_NAME} ${USER}

install-config:
	mkdir -p /etc/house_de.d
	cp ./etc/house_de.d/template.json /etc/house_de.d/${USER}.json
	printf "{\n\t\"std_user\": \"${USER}\",\n\t\"welcome_msg\": \"Welcome, please authenticate as ${USER}\"\n}\n" >> /etc/house_de.json

install-bin: release
	mkdir -p /usr/bin
	mv -f ./target/release/${APP_NAME} /usr/bin
	chmod 755 /usr/bin/${APP_NAME}

install-all: install-bin install-config add-shell

remove-shell:
	sed -i "/\/usr\/bin\/${APP_NAME}/d" /etc/shells

unset-shell:
	usermod --shell /usr/bin/sh ${USER}

uninstall-config:
	rm -r -f /etc/house_de.d
	rm -f /etc/house_de.json

uninstall-bin:
	rm -f /usr/bin/${APP_NAME}

uninstall-all: uninstall-bin uninstall-config remove-shell