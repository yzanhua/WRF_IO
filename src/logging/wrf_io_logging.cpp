#include "wrf_io_logging.hpp"
// #include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "wrf_io.hpp"
using wrf_io_logging::log;

#ifdef WRF_IO_DEBUG
using std::cout;
using std::endl;
using std::localtime;
using std::put_time;
using std::string;
using std::chrono::system_clock;
static int indent_level = 0;
#endif

int log::rank      = -1;
int log::log_level = -1;

void log::init (int rank, int log_level) {
#ifdef WRF_IO_DEBUG
    log::rank      = rank;
    log::log_level = log_level;
#endif
}  // end log::init

void log::print (char *msg, int line, char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    if (rank0_only && rank != 0) return;
    if (level == -1) { level = (indent_level + 1) * 10; }
    if (level < 0 || level > log_level) return;
    string indent ((indent_level + 1) * 2, ' ');

    // Get the current time
    auto now           = system_clock::to_time_t (system_clock::now ());
    std::tm *localTime = localtime (&now);
    cout << "LOG: " << put_time (localTime, "%d-%m %H:%M:%S") << " | " << indent << file << ":"
         << line << " | " << msg << endl;
#endif
}  // end log::print

void log::print (string msg, int line, char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    log::print (msg.c_str (), line, file, level, rank0_only);
#endif
}  // end log::print

// void log::enter (int line, const char *func, const char *file, int level, bool rank0_only) {
void log::enter (
    const char *msg, int line, const char *func, const char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    if (rank0_only && rank != 0) return;
    indent_level++;
    if (level == -1) { level = indent_level * 10; }
    if (level < 0 || level > log_level) return;

    string indent (indent_level * 2, ' ');
    string enter = " | " + indent + "Enter ";

    // Get the current time
    auto now           = system_clock::to_time_t (system_clock::now ());
    std::tm *localTime = localtime (&now);
    if (msg == nullptr) {
        cout << "LOG: " << put_time (localTime, "%d-%m %H:%M:%S") << enter << file << ":" << func
             << ":" << line << endl;
    } else {
        cout << "LOG: " << put_time (localTime, "%d-%m %H:%M:%S") << enter << file << ":" << func
             << ":" << line << " | " << msg << endl;
    }

#endif
}
void log::enter (int line, const char *func, const char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    log::enter (nullptr, line, func, file, level, rank0_only);
#endif
}

// void log::leave (int line, const char *func, const char *file, int level, bool rank0_only) {
void log::leave (
    const char *msg, int line, const char *func, const char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    if (rank0_only && rank != 0) return;
    indent_level--;
    if (level == -1) { level = (indent_level + 1) * 10; }
    if (level < 0 || level > log_level) return;

    string indent ((indent_level + 1) * 2, ' ');
    string leave = " | " + indent + "Leave ";

    // Get the current time
    auto now           = system_clock::to_time_t (system_clock::now ());
    std::tm *localTime = localtime (&now);
    if (msg == nullptr) {
        cout << "LOG: " << put_time (localTime, "%d-%m %H:%M:%S") << leave << file << ":" << func
             << ":" << line << endl;
    } else {
        cout << "LOG: " << put_time (localTime, "%d-%m %H:%M:%S") << leave << file << ":" << func
             << ":" << line << " | " << msg << endl;
    }

#endif
}

void log::leave (int line, const char *func, const char *file, int level, bool rank0_only) {
#ifdef WRF_IO_DEBUG
    log::leave (nullptr, line, func, file, level, rank0_only);
#endif
}
