#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

#include "main.h"
#include "utils.h"

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

    bool isCharging(std::string batterySysfsDir) {
        return utils::readSysfsFileString(batterySysfsDir + "/status") == "Charging";
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

    void loop() {
        int capacity = 0, glyphIndex = -1;
        bool charging = true;
        std::string batterySysfsDir = getBatterySysfsDir();

        while (!batterySysfsDir.empty()) {
            capacity = utils::readSysfsFileInt(batterySysfsDir + "/capacity");
            charging = isCharging(batterySysfsDir);

            if (capacity != -1) {
                if (charging && glyphIndex != -1 && glyphIndex < 4)
                    glyphIndex++;
                else
                    glyphIndex = getCurrentChargeGlyphIndex(capacity);

                printBuffer(getBatteryGlyph(glyphIndex) + " " + std::to_string(capacity) + "%"
                        + (charging ? "+" : ""), "battery");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    }
}
