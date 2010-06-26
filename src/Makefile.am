INCLUDES = $(WARN_CFLAGS) \
           $(DISABLE_DEPRECATED_CFLAGS) \
           $(GTK_CFLAGS) \
           $(GTKFORM_CFLAGS)

LIBS = $(GTK_LIBS) \
       $(GTKFORM_LIBS)

lib_LTLIBRARIES = libgtkmaskedentry.la

if LIBGTKFORM_FOUND
GTKFORM_C = gtkformwidgetmaskedentry.c
GTKFORM_H = gtkformwidgetmaskedentry.h
libgtkformmodulesdir = `pkg-config --variable=modulesdir libgtkform`
else
GTKFORM_C =
GTKFORM_H =
libgtkformmodulesdir =
endif

libgtkmaskedentry_la_SOURCES = gtkmaskedentry.c \
                               gtkcellrenderermasked.c \
                               $(GTKFORM_C)

libgtkmaskedentry_la_LDFLAGS = -no-undefined

include_HEADERS = gtkmaskedentry.h \
                  gtkcellrenderermasked.h \
                  $(GTKFORM_H)

install-exec-hook:
	cd $(libgtkformmodulesdir) && \
	ln -s $(libdir)/libgtkmaskedentry.so .

uninstall-hook:
	rm -f $(libgtkformmodulesdir)/libgtkmaskedentry.so
