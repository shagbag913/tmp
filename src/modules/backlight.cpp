#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>

#include "main.h"
#include "utils.h"

namespace fs = std::filesystem;

namespace backlight {

    std::string getBacklightSysfsPath() {
        std::string backlightSysfsPath = "/sys/class/backlight/";
        try {
            for (const auto& entry : fs::directory_iterator(backlightSysfsPath)) {
                return entry.path().string();
            }
        } catch (fs::filesystem_error& e) {
        }

        return "";
    }

    void loop() {
        std::string backlightSysfsPath = getBacklightSysfsPath();
        int maxBrightness = utils::readSysfsFileInt(backlightSysfsPath + "/max_brightness");

        while (true) {
            int currentBrightness = utils::readSysfsFileInt(backlightSysfsPath + "/brightness");
            if (currentBrightness != -1) {
                std::string brightnessString = std::to_string(utils::getPercentage(currentBrightness, maxBrightness));
                printBuffer(" " + brightnessString + "%", "backlight");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
}
