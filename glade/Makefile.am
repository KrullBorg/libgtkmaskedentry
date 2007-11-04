SUBDIRS = icons

moduledir = `pkg-config --variable=moduledir gladeui-1.0`

catalogsdir = `pkg-config --variable=catalogdir gladeui-1.0`

catalogs_DATA = gtkmaskedentry.xml

EXTRA_DIST = $(catalogs_DATA)

install-exec-hook:
	cd $(moduledir) && \
	ln -s $(libdir)/libgtkmaskedentry.so .

uninstall-hook:
	rm -f $(moduledir)/libgtkmaskedentry.so
