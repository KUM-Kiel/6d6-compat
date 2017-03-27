PREFIX ?= /usr/local

install: 6d6copy 6d6read
	@mkdir -p "$(PREFIX)/bin"
	install -m 4755 6d6info "$(PREFIX)/bin"
	install -m 4755 6d6copy "$(PREFIX)/bin"
	install -m 4755 6d6mseed "$(PREFIX)/bin"
	install -m 0755 6d6read "$(PREFIX)/bin"
