AC_PREREQ(2.59)
AC_INIT([GtkMaskedEntry], [0.0.3], [azagli@inwind.it])
AC_CONFIG_SRCDIR([src/gtkmaskedentry.c])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AM_MAINTAINER_MODE

AC_LIBTOOL_WIN32_DLL

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_CANONICAL_SYSTEM

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

GTK_DOC_CHECK(1.0)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.6.0)
PKG_CHECK_EXISTS(gladeui-1.0 >= 3.2.0, [GLADEUI_FOUND=yes], [GLADEUI_FOUND=no])

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

if test $GLADEUI_FOUND = yes; then
AC_CONFIG_FILES([
  glade/Makefile
  glade/icons/Makefile
  glade/icons/16x16/Makefile
  glade/icons/22x22/Makefile
])
fi

AC_OUTPUT