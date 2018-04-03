AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -std=c++11 -g -Wall -W -Wall -Wformat -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long -Wno-gnu-zero-variadic-macro-arguments
#AM_LDFLAGS = -licui18n -licuuc -licudata
CXX = clang++
# CXX = g++

lib_LIBRARIES = lib/libvte.a lib/libdebugscreen.a
lib_libvte_a_SOURCES = src/vte.cc src/screen.cc src/unicode.cc
lib_libdebugscreen_a_SOURCES = src/debug_screen.cc src/screen.cc

bin_PROGRAMS = bin/test
bin_test_SOURCES = src/test.cc
bin_test_LDADD = lib/libvte.a lib/libdebugscreen.a

man_MANS = man/vte.1

