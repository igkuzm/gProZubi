## Process this file with automake to produce Makefile.in

SUBDIRS = prozubilib

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gprozubi

gprozubi_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

gprozubi_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -L prozubilib/. -lprozubilib -lcurl
