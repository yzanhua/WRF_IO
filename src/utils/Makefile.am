SUFFIXES = .o .cpp .hpp

AM_CPPFLAGS  = -I${top_srcdir}/src
AM_CPPFLAGS += -I${top_builddir}/src
AM_CPPFLAGS += -I${top_srcdir}/src/io_drivers
AM_CPPFLAGS += -I${top_srcdir}/src/utils
AM_CPPFLAGS += -I${top_srcdir}/src/logging

noinst_LTLIBRARIES = libwrf_utils.la

C_SRCS =    attribute.cpp \
            dimension.cpp \
            err.cpp \
            file.cpp \
            variable.cpp \
            wrf_io_utils.cpp \


H_SRCS =    json.hpp \
            wrf_io_utils.hpp \


libwrf_utils_la_SOURCES = $(C_SRCS) $(H_SRCS)

CLEANFILES = core core.* *.gcda *.gcno *.gcov gmon.out
