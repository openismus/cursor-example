BUILT_SOURCES = cursor-example-resources.h cursor-example-resources.c

bin_PROGRAMS = cursor-example
cursor_example_SOURCES =	\
	$(BUILT_SOURCES)	\
	cursor-data.c		\
	cursor-data.h		\
	cursor-example.c	\
	cursor-example.h	\
	main.c

cursor_example_CPPFLAGS =	\
	$(GIO_CFLAGS)		\
	$(GTK_CFLAGS)		\
	$(EDS_CFLAGS)		\
	$(WARN_CFLAGS)		\
	$(AM_CPPFLAGS)
cursor_example_CFLAGS   = $(libtheater_diagnostics_la_CFLAGS)
cursor_example_LDFLAGS  = $(GIO_LIBS) $(GTK_LIBS) $(EDS_LIBS)
cursor_example_LDADD    =

# Generate resources
cursor-example-resources.h: cursor-example.gresources.xml cursor-example.glade
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(srcdir)/cursor-example.gresources.xml \
		--target=$@ --sourcedir=$(srcdir) --c-name _cursor_example --generate-header
cursor-example-resources.c: cursor-example.gresources.xml cursor-example.glade
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(srcdir)/cursor-example.gresources.xml \
		--target=$@ --sourcedir=$(srcdir) --c-name _cursor_example --generate-source

EXTRA_DIST = cursor-example.glade cursor-example.gresources.xml
CLEANFILES = $(BUILT_SOURCES)
