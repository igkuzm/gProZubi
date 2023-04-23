## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gprozubi

gprozubi_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	prozubilib/prozubilib.c \
	prozubilib/kdata2/kdata2.c \
	prozubilib/kdata2/cYandexDisk/cYandexDisk.c\
	prozubilib/kdata2/cYandexDisk/cJSON.c\
	prozubilib/kdata2/cYandexDisk/uuid4.c

gprozubi_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -lsqlite3 -lcurl

