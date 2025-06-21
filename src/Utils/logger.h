//
// Created by Bilawal Ahmed on 16/Jun/2025.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

template <typename ...Args>
void mylog(const std::string& format, Args... args) {
    const size_t bufferNeeded = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;

    char *buffer = new (std::nothrow) char[bufferNeeded];
    if (!buffer) {
        return;
    }

    std::snprintf(buffer, bufferNeeded, format.c_str(), args...);

    std::ofstream logFile("log.txt", std::ios::app);
    logFile << buffer << std::endl;
    delete[] buffer;
}

#endif //LOGGER_H
