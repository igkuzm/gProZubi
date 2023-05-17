dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(gprozubi, 0.1)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CXX

LT_INIT

AC_CANONICAL_HOST
case "${host_os}" in
	cygwin*|mingw*)
	;;
	darwin*)
	;;
	*)
		webkit_modules="webkit2gtk-4.0"
		PKG_CHECK_MODULES(WEBKIT, [$webkit_modules])
		AC_SUBST(WEBKIT_CFLAGS)
		AC_SUBST(WEBKIT_LIBS)
	;;
esac

#gtk_modules="gtk+-2.0 >= 2.0.0"
gtk_modules="gtk+-3.0"
PKG_CHECK_MODULES(GTK, [$gtk_modules])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GETTEXT_PACKAGE=gprozubi
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

dnl Add the languages which your application supports here.
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
