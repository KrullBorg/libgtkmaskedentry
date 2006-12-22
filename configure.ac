AC_PREREQ(2.59)
AC_INIT([GtkMaskedEntry], [0.0.3], [azagli@inwind.it])
AC_CONFIG_SRCDIR([src/gtkmaskedentry.c])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AM_MAINTAINER_MODE

AC_LIBTOOL_WIN32_DLL

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
LT_INIT

AC_CANONICAL_SYSTEM

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

GTK_DOC_CHECK(1.0)

GTK_REQUIRED=2.6.8

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Output files
AC_CONFIG_FILES([
  libgtkmaskedentry.pc
  Makefile 
  src/Makefile
  tests/Makefile
  docs/Makefile
  docs/reference/Makefile
  docs/reference/version.xml
])
AC_OUTPUT
