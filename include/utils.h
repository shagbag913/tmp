#include <iostream>

namespace utils {
    int readSysfsFileInt(std::string fileName);
    std::string readSysfsFileString(std::string filename);
    int getPercentage(int x, int y);
}

