AC_INIT([game], [1.0])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_PROG_CC
AC_PROG_RANLIB

# Find GTK and Cairo
PKG_CHECK_MODULES([GTK], [gtk+-3.0 cairo], [], [
  AC_MSG_ERROR([GTK+3 and Cairo are required])
])

# Export flags
CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/include -I\$(top_srcdir)/lib/cjson"
CFLAGS="$CFLAGS -O2 -g"
LIBS="$LIBS -lm"

# Substitute the pkg-config flags
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

# Generate Makefiles
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  lib/cjson/Makefile
])
AC_OUTPUT
