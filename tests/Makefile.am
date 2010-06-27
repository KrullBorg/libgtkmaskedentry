INCLUDES = \
	$(WARN_CFLAGS) \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GTKFORM_CFLAGS) \
	-I../src

LDADD = ../src/libgtkmaskedentry.la \
        $(GTKFORM_LIBS)

if LIBGTKFORM_FOUND
GTKFORM_NOINST = plugin_gtkform
else
GTKFORM_NOINST = 
endif

noinst_PROGRAMS = \
	masked_entry \
	$(GTKFORM_NOINST)

plugin_gtkform_SOURCES = plugin_gtkform.c
