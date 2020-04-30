#include <iostream>
#include <fstream>

#include "utils.h"

namespace utils {

    int readSysfsFileInt(std::string filename) {
        std::ifstream file(filename);
        std::string buf;
        if (getline(file, buf))
            try {
                return std::stoi(buf);
            } catch (...) {
            }
        return -1;
    }

    std::string readSysfsFileString(std::string filename) {
        std::ifstream file(filename);
        std::string buf;
        if (getline(file, buf))
            return buf;
        return "";
    }
}
