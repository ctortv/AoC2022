AC_INIT([aoc2022], m4_esyscmd_s(echo $(git describe)$(git status --porcelain | awk '{if ($1 == "M") {print "-aoc";exit}}')), ["83553121+ctortv@users.noreply.github.com"])

AC_CANONICAL_HOST

AC_CONFIG_MACRO_DIR([autoconf-archive/m4])

AC_ENABLE_SHARED
AC_DISABLE_STATIC
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CC_STDC

AM_PROG_AR
LT_INIT

AC_SUBST([AM_CFLAGS], ["-std=c17 -v -g -O -Wextra -Wfloat-equal -Wunused -Wall -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wunreachable-code -Wno-unused-function -Wno-cpp -Werror -pedantic -pedantic-errors -Waggregate-return -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimplicit -Wimport -Winit-self -Winline -Winvalid-pch -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpadded -Wparentheses -Wredundant-decls -Wreturn-type -Wsequence-point -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-default -Wswitch-enum -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 ])

AC_OUTPUT

