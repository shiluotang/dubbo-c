ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Wall
AM_CFLAGS	=
AM_CXXFLAGS	=
AM_LDFLAGS	=

lib_LTLIBRARIES = libdubbo-c.la

libdubbo_c_la_SOURCES = \
						./src/stringbuilder.c \
						./src/debug.c \
						./src/networkstream.c \
						./src/dubbo-client.c

check_PROGRAMS	= \
				  test1 \
				  test2

test1_SOURCES	= ./tests/test1.c
test1_LDADD		= libdubbo-c.la
test1_LDFLAGS	= -all-static

test2_SOURCES	= ./tests/test2.c \
				  ./tests/stopwatch.c
test2_LDADD		= libdubbo-c.la
test2_LDFLAGS	= -all-static

TESTS = $(check_PROGRAMS)
