#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

#include "main.h"

namespace fs = std::filesystem;

namespace battery {

    std::string getBatterySysfsDir() {
        try {
            std::string path = "/sys/class/power_supply/";
            for (const auto &entry : fs::directory_iterator(path))
                if (entry.path().string().find("BAT") != std::string::npos)
                    return entry.path();
        } catch (fs::filesystem_error& e) {
            // Do nothing for now, TODO: add logging, disable module
        }
        return "";
    }

    int readCapacityFile() {
        std::ifstream capacityFile;
        std::string capacity;

        capacityFile.open(getBatterySysfsDir() + "/capacity");
        if (!capacityFile.is_open()) {
            // TODO: logging
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
            // TODO: logging
            return false;
        }
        std::getline(statusFile, status);

        return status == "Charging";
    }

    int getCurrentChargeGlyphIndex(int currentCharge) {
        if (currentCharge >= 90)
            return 4;
        else if (currentCharge >= 75)
            return 3;
        else if (currentCharge >= 50)
            return 2;
        else if (currentCharge >= 25)
            return 1;
        else
            return 0;
    }

    std::string getBatteryGlyph(int index) {
        char const* glyphs[] = {"", "", "", "", ""};

        if (index < 0 || index > 4)
            return glyphs[0];

        return glyphs[index];
    }

    void startLoop() {
        std::string statusString;
        int previousCapacity = 0, currentCapacity = 0, glyphIndex = -1;
        bool wasCharging = false, nowCharging = true;

        while (true) {
            currentCapacity = readCapacityFile();
            nowCharging = isCharging();

            if (previousCapacity != currentCapacity || nowCharging != wasCharging || nowCharging) {
                previousCapacity = currentCapacity;
                wasCharging = nowCharging;

                if (nowCharging && glyphIndex != -1) {
                    if (glyphIndex < 4)
                        glyphIndex++;
                    else
                        glyphIndex = getCurrentChargeGlyphIndex(currentCapacity);
                } else
                    glyphIndex = getCurrentChargeGlyphIndex(currentCapacity);


                statusString = getBatteryGlyph(glyphIndex) + " "
                        + std::to_string(currentCapacity) + "%" + (nowCharging ? "+" : "");
                printBuffer(statusString, "battery");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    }

    std::thread start() {
        std::thread t(startLoop);
        return t;
    }
}
