#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(vtutils, 0.1)

# define a "--[en|dis]able-widechar" flag
AC_ARG_ENABLE(
    [widechar],
    [AS_HELP_STRING([--disable-widechar], [disable wide character encoding])],
    [],
    [enable_widechar=yes])
## Output a message indicating we're checking the widechar flag
AC_MSG_CHECKING( [whether wide cahracters are disabled] )
AS_IF([test "x${enable_widechar}" = "xyes"], AC_MSG_RESULT([no]), AC_MSG_RESULT([yes]))
AS_IF(
    [test "x${enable_widechar}" = "xyes"],
    [PKG_CHECK_MODULES([curses], [ncursesw] >= 6.0)],
    [PKG_CHECK_MODULES([curses], [ncurses] >= 6.0)])

# define a "--[en|dis]able-debug" flag
AC_ARG_ENABLE(
    [debug],
    [AS_HELP_STRING([--enable-debug], [enable a debug build])])
# Output a message indicating we're checking the debug flag
AC_MSG_CHECKING([whether debug mode is enabled])
AS_IF([test "x${enable_debug}" = "xyes"], AC_MSG_RESULT([yes]), AC_MSG_RESULT([no]))
AM_CONDITIONAL([DEBUG],[test "x${enable_debug}" = "xyes"])

AC_CONFIG_SRCDIR([src/vte.cc])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
: ${CFLAGS=""}
: ${CXXFLAGS=""}
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([fenv.h float.h inttypes.h limits.h locale.h stddef.h stdint.h stdlib.h string.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT32_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

