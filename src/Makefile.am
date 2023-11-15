## Process this file with automake to produce Makefile.in

SUBDIRS = prozubilib

AM_CFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

if WIN32
CURL_LINK = -Lprozubilib/kdata2/cYandexDisk/curl/windows/win32/bin
endif

if WIN64
CURL_LINK = -Lprozubilib/kdata2/cYandexDisk/curl/windows/win64/bin
endif

if !WINDOWS
SQLITE_LINK = -lsqlite3
endif

bin_PROGRAMS = gprozubi
gprozubi_SOURCES = \
	main.c \
	interface.c interface.h \
	ZubFormula.c ZubFormula.h \
	Nomenklatura.c Nomenklatura.h \
	Xray.c Xray.h \
	TextEdit.c TextEdit.h \
	Calendar.c Calendar.h \
	ComboBox.c ComboBox.h \
	callbacks.c callbacks.h
gprozubi_CFLAGS = $(GTK_CFLAGS)
gprozubi_LDADD = $(INTLLIBS) $(GTK_LIBS) -lm \
								 prozubilib/*.o \
								 prozubilib/kdata2/*.o \
								 prozubilib/kdata2/cYandexDisk/*.o \
								 $(SQLITE_LINK) \
								 -lcurl $(CURL_LINK) -lstdc++
