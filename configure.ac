AC_INIT([wrf_io], [1.0])

AM_INIT_AUTOMAKE([foreign subdir-objects])
# AM_SILENT_RULES([yes])
# AM_MAINTAINER_MODE([enable])

AC_CONFIG_MACRO_DIRS([m4])

UD_PROG_M4

# Add options for DEBUG
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
                    [Enable WRF_IO internal debug mode.
                     @<:@default: disabled@:>@])],
    [debug=${enableval}], [debug=no]
)
AM_CONDITIONAL(WRF_IO_DEBUG, [test "x$debug" = xyes])
if test "x${debug}" = xyes; then
   dnl add -g flag if not presented
   dnl remove all -O and -fast flags
   dnl add -O0 to all flags
   # check exit status of grep command is more portable than using -q
   str_found=`echo "${CXXFLAGS}" | ${EGREP} -- "-g"`
   if test "x$?" != x0 ; then
      CXXFLAGS="$CXXFLAGS -g"
   fi
   CXXFLAGS=`echo $CXXFLAGS | ${SED} 's/-O. *//g' | ${SED} 's/-fast *//g'`
   CXXFLAGS="$CXXFLAGS -O0"

   str_found=`echo "${CFLAGS}" | ${EGREP} -- "-g"`
   if test "x$?" != x0 ; then
      CFLAGS="$CFLAGS -g"
   fi
   CFLAGS=`echo $CFLAGS | ${SED} 's/-O. *//g' | ${SED} 's/-fast *//g'`
   CFLAGS="$CFLAGS -O0"

   unset str_found
   AC_DEFINE([WRF_IO_DEBUG], [1], [Enable debug mode])
fi

# Add options for MPI
MPI_INSTALL=
AC_ARG_WITH(mpi,
   [AS_HELP_STRING([--with-mpi=/path/to/implementation],
                   [The installation prefix path for MPI implementation.])
   ],[ dnl this clause is run when --with-mpi or --without-mpi is used
   if test "x${withval}" = xno ; then
      AC_MSG_ERROR([
      -----------------------------------------------------------------------
        This benchmark is built on top of MPI. Configure option --without-mpi or
        --with-mpi=no should not be used. Abort.
      -----------------------------------------------------------------------])
   elif test "x${withval}" = x ; then
      AC_MSG_ERROR(--with-mpi is set but the value is NULL)
   elif test "x${withval}" != xyes && test ! -d "${withval}" ; then
      # user may use --with-mpi without an argument, which results in withval
      # being "yes". This case is OK and we simply take no action, as H5VL_log
      # requires MPI compilers and will check them.
      AC_MSG_ERROR(Directory '${withval}' specified in --with-mpi does not exist or is not a directory)
   fi
   MPI_INSTALL=${withval}
   ]
)

AC_ARG_VAR(MPICC,  [MPI C compiler, @<:@default: CC@:>@])
AC_ARG_VAR(MPICXX, [MPI C++ compiler, @<:@default: CXX@:>@])

ac_user_MPICC=$MPICC
ac_user_MPICXX=$MPICXX
if test "x$MPICC"  = x && test "x$CC"  != x ; then ac_user_MPICC=$CC   ; fi
if test "x$MPICXX" = x && test "x$CXX" != x ; then ac_user_MPICXX=$CXX ; fi

CANDIDATE_MPICC="${MPICC} mpicc mpicc_r"
CANDIDATE_MPICXX="${MPICXX} mpicxx mpic++ mpiCC mpcxx mpc++ mpicxx_r mpiCC_r mpcxx_r mpic++_r mpc++_r"
dnl add GNU MPI compilers
CANDIDATE_MPICC="$CANDIDATE_MPICC mpigcc mpgcc mpigcc_r mpgcc_r"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX mpig++ mpg++ mpig++_r mpg++_r"
dnl add IBM MPI compilers
CANDIDATE_MPICC="$CANDIDATE_MPICC mpcc_r mpcc mpixlc_r mpixlc"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX mpCC_r mpCC mpixlcxx_r mpixlcxx mpixlC_r mpixlC"
dnl add IBM BGL MPI compilers
CANDIDATE_MPICC="$CANDIDATE_MPICC blrts_xlc mpxlc_r mpxlc"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX blrts_xlC mpxlC_r mpxlC mpixlc++ mpxlcxx mpxlc++ mpxlCC mpixlc++_r mpxlcxx_r mpxlc++_r mpxlCC_r"
dnl add Fujitsu MPI compilers
CANDIDATE_MPICC="$CANDIDATE_MPICC mpifccpx"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX mpiFCCpx"
dnl add Cray MPI compiler wrappers
CANDIDATE_MPICC="$CANDIDATE_MPICC cc"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX CC"
dnl add Intel MPI compiler wrappers
CANDIDATE_MPICC="$CANDIDATE_MPICC mpiicc icc"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX mpiicpc mpiicxx mpiic++ mpiiCC icpc"
dnl add PGI MPI compiler wrappers
CANDIDATE_MPICC="$CANDIDATE_MPICC mpipgcc mppgcc"
CANDIDATE_MPICXX="$CANDIDATE_MPICXX mpipgCC mppgCC"

dnl find the full path of MPICC from CANDIDATE_MPICC and MPI_INSTALL
if test "x${ac_user_MPICC}" = x ; then
   dnl if MPICC or CC has not been set by users, then search from
   dnl CANDIDATE_MPICC, and find the full path of MPICC
   UD_MPI_PATH_PROGS([MPICC], [$CANDIDATE_MPICC])
else
   dnl check whether user specified MPICC is valid
   UD_MPI_PATH_PROG([MPICC], [$ac_user_MPICC])
fi

if test "x${MPICC}" = x ; then
   if test "x$ac_user_MPICC" = x ; then
      ERR_MSG="No MPI C compiler can be found"
   else
      ERR_MSG="Specified MPI C compiler \"$ac_user_MPICC\" cannot be found"
   fi
   if test "x$MPI_INSTALL" != x ; then
      ERR_MSG="$ERR_MSG under $MPI_INSTALL"
   fi
   AC_MSG_ERROR([
   -----------------------------------------------------------------------
     $ERR_MSG
     H5VL_log requires a working MPI C compiler. Please specify the
     location of an MPI C compiler, either in the MPICC environment
     variable (not CC variable) or through --with-mpi configure flag.
     Abort.
   -----------------------------------------------------------------------])
fi
CC=${MPICC}
AC_PROG_CC

dnl find the full path of MPICXX from CANDIDATE_MPICXX and MPI_INSTALL
if test "x${ac_user_MPICXX}" = x ; then
   dnl if MPICXX or CXX has not been set by users, then search from
   dnl CANDIDATE_MPICXX, and find the full path of MPICXX
   UD_MPI_PATH_PROGS([MPICXX], [$CANDIDATE_MPICXX])
else
   dnl check whether user specified MPICXX is valid
   UD_MPI_PATH_PROG([MPICXX], [$ac_user_MPICXX])
fi
if test "x${MPICXX}" = x ; then
   if test "x$ac_user_MPICXX" = x ; then
      ERR_MSG="No MPI C++ compiler can be found"
   else
      ERR_MSG="Specified MPI C++ compiler \"$ac_user_MPICXX\" cannot be found"
   fi
   if test "x$MPI_INSTALL" != x ; then
      ERR_MSG="$ERR_MSG under $MPI_INSTALL"
   fi
   AC_MSG_ERROR([
   -----------------------------------------------------------------------
     $ERR_MSG
     H5VL_log requires a working MPI C++ compiler. Please specify the
     location of an MPI C++ compiler, either in the MPICXX environment
     variable (not CXX variable) or through --with-mpi configure flag.
     Abort.
   -----------------------------------------------------------------------])
fi
CXX=${MPICXX}
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX(17, , optional)
AM_CONDITIONAL(HAVE_CXX17, [test "x${HAVE_CXX17}" = x1])

AX_CXX_COMPILE_STDCXX(11, , mandatory)

dnl Set output variable CPP to a command that runs the C preprocessor.
dnl Some C compilers require -E to be used as C preprocessor.
AC_PROG_CPP

AC_SYS_LARGEFILE
AC_C_CHAR_UNSIGNED
AC_C_BIGENDIAN
AM_CONDITIONAL(IS_BIGENDIAN, [test x$ac_cv_c_bigendian = xyes])
AC_SUBST(ac_cv_c_bigendian)dnl for src/utils/pnetcdf-config.in

dnl check if MPICXX works for basic MPI call: MPI_Comm_rank()
AC_LANG_PUSH(C++)
AC_CHECK_FUNC([MPI_Comm_rank], [],
   dnl maybe -lmpi is needed at link stage
   [AC_SEARCH_LIBS([MPI_Comm_rank], [mpi mpi++ mpich mpichcxx mpi_cxx], [],
                   [AC_MSG_ERROR([
   -----------------------------------------------------------------------
     Invalid MPI compiler specified or detected: "${MPICXX}"
     A working MPI C++ compiler is required. Please specify the location
     of one either in the MPICXX environment variable (not CXX variable)
     or through --with-mpi configure flag. Abort.
   -----------------------------------------------------------------------])
])])

AC_CHECK_FUNC([MPI_File_open], [],
   dnl maybe -lmpi++ is needed at link stage
   [AC_SEARCH_LIBS([MPI_File_open], [mpio], [],
                   [AC_MSG_ERROR([
   -----------------------------------------------------------------------
     The underneath MPI implementation does not support MPI-IO.
     This benchmark requires MPI-IO support to work properly. Abort.
   -----------------------------------------------------------------------])
])])
AC_LANG_POP(C++)

if test "x${HAVE_CXX17}" = x1 ; then
   CXXFLAGS="$CXXFLAGS -std=c++17"
else
   CXXFLAGS="$CXXFLAGS -std=c++11"
fi

# Add options for HDF5
# Check for HDF5
have_hdf5=no
AC_ARG_WITH([hdf5],
   [AS_HELP_STRING([--with-hdf5@<:@=INC,LIB | =DIR@:>@],
                   [Enable HDF5 feature and provide the HDF5 installation path(s):
    --with-hdf5=INC,LIB for include and lib paths separated by a comma.
    --with-hdf5=DIR for the path containing include/ and lib/ subdirectories.
    @<:@default: disabled@:>@
   ])], [
   case $withval in
     *,*)
        hdf5_inc="`echo $withval |cut -f1 -d,`"
        hdf5_lib="`echo $withval |cut -f2 -d, -s`"
        ;;
     *)
        if test -n "$withval"; then
          hdf5_inc="$withval/include"
          hdf5_lib="$withval/lib"
        fi
        ;;
   esac
   if test "x$hdf5_inc" != x ; then
      if test "x$CPPFLAGS" = x ; then
         CPPFLAGS="-I$hdf5_inc"
      elif ! echo "${CPPFLAGS}" | ${EGREP} -q -w -- "-I$hdf5_inc" ; then
         # append only if not already appear in CPPFLAGS
         CPPFLAGS="$CPPFLAGS -I$hdf5_inc"
      fi
   fi
   if test "x$hdf5_lib" != x ; then
      if test "x$LDFLAGS" = x ; then
         LDFLAGS="-L$hdf5_lib"
      elif ! echo "${LDFLAGS}" | ${EGREP} -q -w -- "-L$hdf5_lib" ; then
         # append only if not already appear in LDFLAGS
         LDFLAGS="$LDFLAGS -L$hdf5_lib"
      fi
   fi

   AC_CHECK_HEADER([hdf5.h], [have_hdf5=yes], [have_hdf5=no])
   if test "x$have_hdf5" = xno ; then
      AC_MSG_ERROR([
      -----------------------------------------------------------------------
      Missing HDF5-header files 'hdf5.h' required to build WRF_IO. Use
      configure command-line option --with-hdf5=/path/to/implementation
      to specify the location of HDF5 installation. Abort.
      -----------------------------------------------------------------------])
   else
      AC_MSG_CHECKING([HDF5 library version])
      AC_COMPUTE_INT([hdf5_major], [H5_VERS_MAJOR],   [[#include <hdf5.h>]])
      AC_COMPUTE_INT([hdf5_minor], [H5_VERS_MINOR],   [[#include <hdf5.h>]])
      AC_COMPUTE_INT([hdf5_patch], [H5_VERS_RELEASE], [[#include <hdf5.h>]])
      hdf5_version=${hdf5_major}.${hdf5_minor}.${hdf5_patch}
      AC_MSG_RESULT([$hdf5_version])
      AC_MSG_CHECKING([whether HDF5 is configured with parallel I/O enabled])
      AC_COMPUTE_INT([h5_parallel], [H5_HAVE_PARALLEL], [[#include <hdf5.h>]], [h5_parallel=0])
      if test "x$h5_parallel" = x1 ; then
         hdf5_parallel_io=yes
      else
         hdf5_parallel_io=no
      fi
      AC_MSG_RESULT([$hdf5_parallel_io])
      if test "x$hdf5_parallel_io" = xno ; then
         AC_MSG_ERROR([
         -----------------------------------------------------------------------
         WRF_IO requires parallel I/O feature enabled in HDF5. However, This
         version of HDF5 library, $hdf5_version, is not configured to support
         parallel I/O. Abort.
         -----------------------------------------------------------------------])
      fi
   fi
   if test "x$have_hdf5" = xyes ; then
      # Check whether HDF5 supports H5S_BLOCK
      AC_CHECK_DECLS([H5S_BLOCK], [], [], [#include <hdf5.h>])
      if test "x$ac_cv_have_decl_H5S_BLOCK" = xyes ; then
         AC_DEFINE([HDF5_HAVE_H5SBLOCK], [1], [Whether HDF5 defines H5S_BLOCK])
      fi
      AC_SUBST(HDF5_LIB_PATH, [$hdf5_lib])
   fi
])
AM_CONDITIONAL(USE_HDF5, [test "x$have_hdf5" = xyes])

have_multi_dset=no
if test "x$have_hdf5" = xyes ; then
   # check for extra libraries used when building HDF5
   extra_libs=
   if test -f $hdf5_lib/libhdf5.settings ; then
      AC_MSG_CHECKING([extra libraries used by HDF5])
      extra_ldflags="`grep ' LDFLAGS:' $hdf5_lib/libhdf5.settings |cut -f2 -d:`"
      extra_H5_ldflags="`grep 'H5_LDFLAGS:' $hdf5_lib/libhdf5.settings |cut -f2 -d:`"
      extra_AM_ldflags="`grep 'AM_LDFLAGS:' $hdf5_lib/libhdf5.settings |cut -f2 -d:`"
      if test "x$extra_H5_ldflags" != x ; then
         extra_ldflags+=$extra_H5_ldflags
      fi
      if test "x$extra_AM_ldflags" != x ; then
         extra_ldflags+=$extra_AM_ldflags
      fi
      extra_libs="`grep 'Extra libraries' $hdf5_lib/libhdf5.settings |cut -f2 -d:`"
      AC_MSG_RESULT([$extra_libs])
   fi
   if test "x$LIBS" = x ; then
      LIBS="-lhdf5 $extra_libs"
   else
      LIBS="-lhdf5 $extra_libs $LIBS"
   fi
   if test "x$LDFLAGS" = x ; then
      LDFLAGS=$extra_ldflags
   else
      LDFLAGS="$extra_ldflags $LDFLAGS"
   fi
   AC_DEFINE([USE_HDF5], [1], [Enable HDF5 I/O method])

   # H5Dwrite_multi() is first introduced in HDF5 1.14.0
   # Need all HDF5 dependent libraries set in LIBS before AC_SEARCH_LIBS
   AC_SEARCH_LIBS([H5Dwrite_multi], [hdf5], [have_multi_dset=yes], [have_multi_dset=no])
   if test "x${have_multi_dset}" = xno ; then
      AC_MSG_WARN([
      ---------------------------------------------------------------
      This HDF5 $hdf5_version library does not support 'multi-dataset'
      APIs. The 'hdf5_md' I/O option is thus disabled.
      ---------------------------------------------------------------])
   else
      AC_DEFINE([HDF5_HAVE_MULTI_DATASET_API], [1], [HDF5 has multi-dataset I/O support])
   fi
   # H5Pset_selection_io() is first introduced in HDF5 1.14.1-2
   AC_SEARCH_LIBS([H5Pset_selection_io], [hdf5])
   if test "x$ac_cv_search_H5Pset_selection_io" = "xnone required" ; then
      AC_DEFINE([HDF5_HAVE_SELECTION_IO], [1], [Whether HDF5 defines H5Pset_selection_io])
   fi
fi
AM_CONDITIONAL(HDF5_HAVE_MULTI_DATASET_API, [test "x$have_multi_dset" = xyes])



# Add options for PnetCDF
# Check for pnc
have_pnc=no
AC_ARG_WITH([pnetcdf],
   [AS_HELP_STRING([--with-pnetcdf@<:@=INC,LIB | =DIR@:>@],
                   [Provide the PnetCDF installation path(s):
    --with-pnetcdf=INC,LIB for include and lib paths separated by a comma.
    --with-pnetcdf=DIR for the path containing include/ and lib/ subdirectories.
    @<:@default: enabled@:>@
   ])], [
   case $withval in
     *,*)
        pnc_inc="`echo $withval |cut -f1 -d,`"
        pnc_lib="`echo $withval |cut -f2 -d, -s`"
        ;;
     *)
        if test -n "$withval"; then
          pnc_inc="$withval/include"
          pnc_lib="$withval/lib"
        fi
        ;;
   esac
   if test "x$pnc_inc" != x ; then
      if test "x$CPPFLAGS" = x ; then
         CPPFLAGS="-I$pnc_inc"
      elif ! echo "${CPPFLAGS}" | ${EGREP} -q -w -- "-I$pnc_inc" ; then
         # append only if not already appear in CPPFLAGS
         CPPFLAGS="$CPPFLAGS -I$pnc_inc"
      fi
   fi
   if test "x$pnc_lib" != x ; then
      if test "x$LDFLAGS" = x ; then
         LDFLAGS="-L$pnc_lib"
      elif ! echo "${LDFLAGS}" | ${EGREP} -q -w -- "-L$pnc_lib" ; then
         # append only if not already appear in LDFLAGS
         LDFLAGS="$LDFLAGS -L$pnc_lib"
      fi
   fi

   AC_CHECK_HEADER([pnetcdf.h], [have_pnc=yes], [have_pnc=no])
   if test "x$have_pnc" = xno ; then
      AC_MSG_ERROR([
      -----------------------------------------------------------------------
      Missing PnetCDF-header files 'pnetcdf.h' required to build WRF_IO. Use
      configure command-line option --with-pnetcdf=/path/to/implementation
      to specify the location of PnetCDF installation. Abort.
      -----------------------------------------------------------------------])
   else
      dnl Check if PnetCDF version is 1.10.0 or later
      AC_MSG_CHECKING([whether PnetCDF version is 1.10.0 or later])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <pnetcdf.h>
      #if (PNETCDF_VERSION_MAJOR*1000000 + PNETCDF_VERSION_MINOR*1000 + PNETCDF_VERSION_PATCH < 1010000)
      #error PnetCDF version is older than 1.10.0
      #endif
      ]])], [pnc_ge_1_10_0=yes], [pnc_ge_1_10_0=no])
      AC_MSG_RESULT([$pnc_ge_1_10_0])

      if test x$pnc_ge_1_10_0 = xno; then
         AC_MSG_ERROR([
         -----------------------------------------------------------------------
         PnetCDF version 1.10.0 and later is required. Abort.
         -----------------------------------------------------------------------])
      fi
      AC_SUBST(PNETCDF_LIB_PATH, [$pnc_lib])
   fi
])
AM_CONDITIONAL(USE_PNETCDF, [test "x$have_pnc" = xyes])

if test "x$have_pnc" = xyes ; then
   if test "x$LIBS" = x ; then
      LIBS="-lpnetcdf"
   else
      LIBS="-lpnetcdf $LIBS"
   fi
   AC_DEFINE([USE_PNETCDF], [1], [Enable PnetCDF I/O method])
fi



dnl libtoolize: `AC_PROG_RANLIB' is rendered obsolete by `LT_INIT'
dnl AC_PROG_RANLIB
LT_INIT

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/wrf_io.hpp])
AC_PROG_CC
AC_CONFIG_FILES(
    Makefile \
    src/Makefile \
    src/io_drivers/Makefile \
    src/utils/Makefile \
    src/logging/Makefile
)
AC_OUTPUT
echo "------------------------------------------------------------------------------"
echo \
"
   ${PACKAGE_NAME} Version ${PACKAGE_VERSION}
"

echo "\

   Compilers: MPICC    = ${MPICC}
              MPICXX   = ${MPICXX}"
if test "x${CPPFLAGS}" != x ; then
   echo "\
              CPPFLAGS = ${CPPFLAGS}"
fi
echo "\
              CFLAGS   = ${CFLAGS}"
if test "${CXXFLAGS}" != x ; then
   echo "\
              CXXFLAGS = ${CXXFLAGS}"
fi
if test "x${LDFLAGS}" != x ; then
   echo "\
              LDFLAGS  = ${LDFLAGS}"
fi
if test "x${LIBS}" != x ; then
   echo "\
              LIBS     = ${LIBS}"
fi
echo "\

   Others:
              HDF5                  - ${have_hdf5}
              HDF5 multi-dset APIs  - ${have_multi_dset}
              PnetCDF               - ${have_pnc}

   Now run 'make' to build the executable.

------------------------------------------------------------------------------"

