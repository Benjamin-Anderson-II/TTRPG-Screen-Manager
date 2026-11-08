#include <iostream>
#include <fstream>
#include <format>

#define LOG_FILE_PATH "debug/tabletop.log"
extern std::ofstream logFile;

template <typename... Args>
void log_message(std::format_string<Args...> fmt, Args&&... args) {
    std::string msg = std::format(fmt, std::forward<Args>(args)...);
    if(logFile.is_open())
        logFile << msg << std::endl;
    else
        std::cerr << "Panic! Log file is not open!!!\n";
}
