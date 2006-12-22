INCLUDES = $(WARN_CFLAGS) \
           $(DISABLE_DEPRECATED_CFLAGS) \
           $(GTK_CFLAGS)

LIBS = $(GTK_LIBS)

lib_LTLIBRARIES = libgtkmaskedentry.la

libgtkmaskedentry_la_SOURCES = gtkmaskedentry.c \
                               gtkcellrenderermasked.c

libgtkmaskedentry_la_LDFLAGS = -no-undefined

include_HEADERS = gtkmaskedentry.h \
                  gtkcellrenderermasked.h
