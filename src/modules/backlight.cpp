#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>

#include "main.h"

namespace fs = std::filesystem;

namespace backlight {

    std::string getBacklightPercentageString() {
        std::string backlightSysfsPath = "/sys/class/backlight/";
        for (const auto& entry : fs::directory_iterator(backlightSysfsPath)) {
            backlightSysfsPath = entry.path().string();
            break;
        }
        if (backlightSysfsPath == "/sys/class/backlight/")
            return "";

        std::ifstream brightness;
        brightness.open(backlightSysfsPath + "/max_brightness");

        std::string buf;
        getline(brightness, buf);
        brightness.close();
        int maxBrightness = std::stoi(buf);

        brightness.open(backlightSysfsPath + "/brightness");
        getline(brightness, buf);
        brightness.close();
        int currentBrightness = std::stoi(buf);

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
        std::string oldBrightnessString, newBrightnessString;

        while (true) {
            newBrightnessString = " " + getBacklightPercentageString() + "%";
            if (newBrightnessString != oldBrightnessString) {
                oldBrightnessString = newBrightnessString;
                printBuffer(newBrightnessString, "backlight");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    std::thread start() {
        std::thread t1(loop);
        return t1;
    }
}

