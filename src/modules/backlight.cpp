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

    std::string getBacklightPercentageString(int maxBrightness, int currentBrightness) {
        float percentage = ((float)currentBrightness / maxBrightness) * 100;
        std::string brightnessPercentageString;

        if (percentage == 100)
            brightnessPercentageString = "100";
        else {
            brightnessPercentageString = std::to_string(percentage);
            if (percentage < 10)
                brightnessPercentageString.resize(1);
            else
                brightnessPercentageString.resize(2);
        }

        return brightnessPercentageString;
    }

    void loop() {
        std::string backlightSysfsPath = getBacklightSysfsPath();
        std::string brightnessString;
        int maxBrightness = utils::readSysfsFileInt(backlightSysfsPath + "/max_brightness");
        int currentBrightness;

        while (true) {
            currentBrightness = utils::readSysfsFileInt(backlightSysfsPath + "/brightness");
            if (currentBrightness != -1) {
                brightnessString = getBacklightPercentageString(maxBrightness, currentBrightness);
                printBuffer(" " + brightnessString + "%", "backlight");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    std::thread start() {
        std::thread t1(loop);
        return t1;
    }
}
