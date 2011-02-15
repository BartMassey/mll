#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(mll,0.1,bart@cs.pdx.edu,,http://wiki.cs.pdx.edu/bartforge/mll)
AC_CONFIG_SRCDIR([mll.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [log])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile
		 nbayes/Makefile
		 neuron/Makefile
		 hamming/Makefile
		 dtree/Makefile])
AC_OUTPUT
