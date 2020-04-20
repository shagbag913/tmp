#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
namespace fs = std::filesystem;

namespace battery {

    std::string getBatterySysfsDir() {
        std::string path = "/sys/class/power_supply/";
        for (const auto &entry : fs::directory_iterator(path))
            if (entry.path().string().find("BAT") != std::string::npos)
                return entry.path();
        return "";
    }

    int readCapacityFile() {
        std::ifstream capacityFile;
        std::string capacity;

        capacityFile.open(getBatterySysfsDir() + "/capacity");
        if (!capacityFile.is_open()) {
            /* Return -1 in case file couldn't be opened */
            return -1;
        }
        std::getline(capacityFile, capacity);

        return std::stoi(capacity);
    }

    bool isCharging() {
        std::ifstream statusFile;
        std::string status;

        statusFile.open(getBatterySysfsDir() + "/status");
        if (!statusFile.is_open()) {
            return false;
        }
        std::getline(statusFile, status);

        return status == "Charging";
    }

    std::string updateStatusFunction() {
        std::string statusString;

        statusString = "  " + std::to_string(readCapacityFile()) + "%";
        if (isCharging()) {
            statusString += "";
        }

        return statusString;
    }
}
