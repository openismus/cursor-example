dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.52)

m4_define(cursor_major_version, 0)
m4_define(cursor_minor_version, 1)
m4_define(cursor_micro_version, 0)
m4_define(cursor_version, cursor_major_version.cursor_minor_version.cursor_micro_version)

AC_INIT([cursor], [cursor_version])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/cursor-example.h])
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE(CURSOR_MAJOR_VERSION, cursor_major_version, [Cursor major version])
AC_SUBST(CURSOR_MAJOR_VERSION, cursor_major_version)
AC_DEFINE(CURSOR_MINOR_VERSION, cursor_minor_version, [Cursor minor version])
AC_SUBST(CURSOR_MINOR_VERSION, cursor_minor_version)
AC_DEFINE(CURSOR_MICRO_VERSION, cursor_micro_version, [Cursor micro version])
AC_SUBST(CURSOR_MICRO_VERSION, cursor_micro_version)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-bzip2])

# Support silent build rules, requires at least automake-1.11. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([dlopen win32-dll disable-static])

AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])
GNOME_MAINTAINER_MODE_DEFINES

dnl ================================================================
dnl Check for gtk-doc
dnl ================================================================
GTK_DOC_CHECK(1.9)

dnl ================================================================
dnl Check for gobject
dnl ================================================================
PKG_CHECK_MODULES(GIO, [gio-2.0 >= 2.32.0])
AC_SUBST(GIO_LIBS)
AC_SUBST(GIO_CFLAGS)

dnl ================================================================
dnl Check for gtk+
dnl ================================================================
PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= 3.4.0])
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

dnl ================================================================
dnl Check for EDS
dnl ================================================================
PKG_CHECK_MODULES(EDS, [
	libedataserver-1.2 >= 3.9.90
	libebook-contacts-1.2 >= 3.9.90
        libedata-book-1.2 >= 3.9.90
	libebook-1.2 >= 3.9.90
])
AC_SUBST(EDS_LIBS)
AC_SUBST(EDS_CFLAGS)

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([
Makefile
src/Makefile
doc/Makefile
doc/version.xml
])

AC_OUTPUT
