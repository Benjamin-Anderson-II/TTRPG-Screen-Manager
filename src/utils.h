#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <format>

#define LOG_FILE_NAME "debug/tabletop.log"
static std::ofstream logFile;

template <typename... Args>
void log_message(std::format_string<Args...>, Args&&...);

#endif
