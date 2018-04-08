AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -std=c++11 -g -O0 -Wall
#CXX = clang++
#CXX = g++

lib_LIBRARIES = lib/libvte.a lib/libdebugscreen.a lib/libcursesscreen.a
lib_libvte_a_SOURCES = src/vte.cc src/screen.cc src/unicode.cc
lib_libdebugscreen_a_SOURCES = src/debug_screen.cc src/screen.cc
lib_libcursesscreen_a_SOURCES = src/curses_screen.cc src/screen.cc
lib_libcursesscreen_a_CXXFLAGS = ${AM_CXXFLAGS} ${curses_CFLAGS}

bin_PROGRAMS = bin/test
bin_test_SOURCES = src/test.cc
bin_test_LDADD = lib/libvte.a lib/libdebugscreen.a lib/libcursesscreen.a ${curses_LIBS}
bin_test_CXXFLAGS = ${AM_CXXFLAGS} ${curses_CFLAGS}

man_MANS = man/vte.1

