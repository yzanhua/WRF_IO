SUFFIXES = .o .cpp .hpp

AM_CPPFLAGS  = -I${top_srcdir}/src
AM_CPPFLAGS += -I${top_builddir}/src
AM_CPPFLAGS += -I${top_srcdir}/src/io_drivers
AM_CPPFLAGS += -I${top_srcdir}/src/utils
AM_CPPFLAGS += -I${top_srcdir}/src/logging

noinst_LTLIBRARIES = libwrf_io_drivers.la

C_SRCS =    wrf_io_drivers.cpp

H_SRCS =    wrf_io_drivers.hpp

if USE_PNETCDF
C_SRCS += io_drivers/wrf_io_drivers_pnc_nb.cpp
H_SRCS += io_drivers/wrf_io_drivers_pnc_nb.hpp
endif


libwrf_io_drivers_la_SOURCES = $(C_SRCS) $(H_SRCS)

CLEANFILES = core core.* *.gcda *.gcno *.gcov gmon.out
