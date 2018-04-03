## Process this file with automake to produce Makefile.in

AM_CXXFLAGS = -std=c++11 -g -Wall -W -Wall -Wformat -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long -Wno-gnu-zero-variadic-macro-arguments
#AM_LDFLAGS = -licui18n -licuuc -licudata
CXX = clang++
# CXX = g++

lib_LIBRARIES = libvte.a libdebugscreen.a
libvte_a_SOURCES = vte.cc screen.cc unicode.cc
libdebugscreen_a_SOURCES = debug_screen.cc screen.cc

bin_PROGRAMS = test
test_SOURCES = test.cc
test_LDADD = libvte.a libdebugscreen.a
