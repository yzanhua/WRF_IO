AM_CPPFLAGS  = -I${top_srcdir}/src
AM_CPPFLAGS += -I${top_srcdir}/src/io_drivers
AM_CPPFLAGS += -I${top_srcdir}/src/utils
AM_CPPFLAGS += -I${top_srcdir}/src/logging
AM_M4FLAGS = -I${srcdir}

M4FLAGS += -I${top_srcdir}/m4

bin_PROGRAMS = wrf_io

H_SRCS =    wrf_io.hpp \


wrf_io_SOURCES =    wrf_io.cpp \


wrf_io_LDADD =  utils/libwrf_utils.la \
                io_drivers/libwrf_io_drivers.la \
                logging/libwrf_logging.la \


SUBDIRS = io_drivers utils logging
DIST_SUBDIRS = io_drivers utils logging
EXTRA_DIST = $(H_SRCS)
