#pragma once

#ifndef _SRC_LOGGING_WRF_IO_LOGGING_HPP__
#define _SRC_LOGGING_WRF_IO_LOGGING_HPP__

#include <string>

namespace wrf_io_logging {
using std::string;
class log {
   public:
    static int rank;
    static int log_level;

    static void init (int rank, int log_level);
    static void print (char *msg, int line, char *file, int level, bool rank0_only);
    static void print (string msg, int line, char *file, int level, bool rank0_only);

    // static void enter (int line, char *func, char *file, int level, bool rank0_only);
    // static void enter (int line, const char *func, char *file, int level, bool rank0_only);
    static void enter (int line, const char *func, const char *file, int level, bool rank0_only);
    static void enter (
        const char *msg, int line, const char *func, const char *file, int level, bool rank0_only);
    // static void leave (int line, char *func, char *file, int level, bool rank0_only);
    static void leave (int line, const char *func, const char *file, int level, bool rank0_only);
    static void leave (
        const char *msg, int line, const char *func, const char *file, int level, bool rank0_only);
};
}  // namespace wrf_io_logging

#define ENTER \
    { wrf_io_logging::log::enter (__LINE__, __func__, __FILE__, -1, true); }
#define LEAVE \
    { wrf_io_logging::log::leave (__LINE__, __func__, __FILE__, -1, true); }

#define ENTER_MSG(A) \
    { wrf_io_logging::log::enter (A, __LINE__, __func__, __FILE__, -1, true); }
#define LEAVE_MSG(A) \
    { wrf_io_logging::log::leave (A, __LINE__, __func__, __FILE__, -1, true); }

#endif  // _SRC_LOGGING_WRF_IO_LOGGING_HPP__