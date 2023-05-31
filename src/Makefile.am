## Process this file with automake to produce Makefile.in

SUBDIRS = prozubilib

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

#lib_LTLIBRARIES = libYDConnect.la
libYDConnect_la_SOURCES = YDConnect.cc webview.h
libYDConnect_la_CXXFLAGS = $(WEBKIT_CFLAGS)
libYDConnect_la_LIBADD = $(WEBKIT_LIBS)

bin_PROGRAMS = gprozubi
gprozubi_SOURCES = \
	main.c \
	interface.c interface.h \
	ZubFormula.c ZubFormula.h \
	Xray.c Xray.h \
	callbacks.c callbacks.h
gprozubi_CFLAGS = $(GTK_CFLAGS)
gprozubi_LDADD = $(INTLLIBS) $(GTK_LIBS) \
								 -Lprozubilib/. -lprozubilib -lcurl -lsqlite3
#-L.libs/ -lYDConnect
