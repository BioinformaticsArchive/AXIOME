AC_INIT([autoqiime], m4_esyscmd([build-aux/git-version-gen .tarball-version]), [andre@masella.name], [autoqiime])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 dist-bzip2])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_LN_S

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

AC_SUBST(VALAFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

GLIB_REQUIRED=2.16.0
PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)

AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GEE, gee-1.0)
PKG_CHECK_MODULES(LIBXML, libxml-2.0)
AC_CHECK_HEADER([math.h], [], [AC_MSG_ERROR([*** math.h is required, install math header files])])
AC_CHECK_LIB([m], [exp], [], [AC_MSG_ERROR([*** exp is required, install math library files])])
AC_CHECK_HEADER([zlib.h], [], [AC_MSG_ERROR([*** zlib.h is required, install zlib header files])])
AC_CHECK_LIB([z], [gzopen], [], [AC_MSG_ERROR([*** gzopen is required, install zlib library files])])
AC_CHECK_HEADER([bzlib.h], [], [AC_MSG_ERROR([*** bzlib.h is required, install bzip2 header files])])
AC_CHECK_LIB([bz2], [BZ2_bzDecompressInit], [], [AC_MSG_ERROR([*** BZ2_bzDecompressInit is required, install bzip2 library files])])
AC_CHECK_HEADER([magic.h], [], [AC_MSG_ERROR([*** magic.h is required, install libmagic header files])])
AC_CHECK_LIB([magic], [magic_open], [], [AC_MSG_ERROR([*** magic_open is required, install libmagic library files])])

AC_SUBST(GEE_CFLAGS)
AC_SUBST(GEE_LIBS)
AC_SUBST(VALA_CCODE_RULE)

AC_CONFIG_FILES([Makefile]) 
AC_OUTPUT