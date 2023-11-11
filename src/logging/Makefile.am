SUFFIXES = .o .cpp .hpp

AM_CPPFLAGS  = -I${top_srcdir}/src
AM_CPPFLAGS += -I${top_builddir}/src
AM_CPPFLAGS += -I${top_srcdir}/src/io_drivers
AM_CPPFLAGS += -I${top_srcdir}/src/utils
AM_CPPFLAGS += -I${top_srcdir}/src/logging

noinst_LTLIBRARIES = libwrf_logging.la

C_SRCS =    wrf_io_logging.cpp \


H_SRCS =    wrf_io_logging.hpp \


libwrf_logging_la_SOURCES = $(C_SRCS) $(H_SRCS)

CLEANFILES = core core.* *.gcda *.gcno *.gcov gmon.out
