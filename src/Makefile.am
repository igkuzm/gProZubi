## Process this file with automake to produce Makefile.in

SUBDIRS = prozubilib

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

#lib_LTLIBRARIES = libYDConnect.la
#libYDConnect_la_SOURCES = YDConnect.cc webview.h
#libYDConnect_la_CXXFLAGS = $(WEBKIT_CFLAGS)
#libYDConnect_la_LIBADD = $(WEBKIT_LIBS)

bin_PROGRAMS = gprozubi
gprozubi_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h
gprozubi_CFLAGS = $(GTK2_CFLAGS)
gprozubi_LDADD = $(INTLLIBS) $(GTK2_LIBS) \
								 -Lprozubilib/. -lprozubilib -lcurl \
								 -L.libs/ -lYDConnect
