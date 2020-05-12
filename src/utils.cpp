#include <iostream>
#include <fstream>

#include "utils.h"

namespace utils {

    int readSysfsFileInt(const std::string& filename) {
        std::ifstream file(filename);
        std::string buf;
        if (getline(file, buf))
            try {
                return std::stoi(buf);
            } catch (...) {
            }
        return -1;
    }

    std::string readSysfsFileString(const std::string& filename) {
        std::ifstream file(filename);
        std::string buf;
        if (getline(file, buf))
            return buf;
        return "";
    }

    int getPercentage(int x, int y) {
        return (int)((float)x / y * 100 + 0.5f);
    }
}
