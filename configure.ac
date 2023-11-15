dnl Process this file with autoconf to produce a configure script.

AC_INIT([gprozubi], [0.1])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_PROG_CC

LT_INIT

AC_CANONICAL_HOST
case "${host_os}" in
    cygwin*|mingw*)
        build_windows=yes;;
    darwin*)
        build_mac=yes;;
    *)
		build_linux=yes;;
esac

case "${host_cpu}" in
    i?86*)
        build_i386=yes;;
    x86_64*)
        build_x86_64=yes;;
    arm64*|aarch64*)
        build_arm64=yes;;
    armv7*)
        build_arm=yes;;
    *)
		;;
esac

AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([WIN32], [test "$build_windows" = "yes" -a "$build_i386" == "yes"])
AM_CONDITIONAL([WIN64], [test "$build_windows" = "yes" -a "$build_x86_64" == "yes"])

gtk_modules="gtk+-2.0 >= 2.0.0"
#gtk_modules="gtk+-3.0"
PKG_CHECK_MODULES(GTK, [$gtk_modules])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/prozubilib/Makefile
])

AC_OUTPUT

