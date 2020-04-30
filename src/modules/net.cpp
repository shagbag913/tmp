#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <sys/stat.h>
#include <vector>

#include "main.h"

namespace fs = std::filesystem;

namespace net {

    std::string formatNetworkString() {
        std::string formattedNetString, operstateCont, wirelessPath;
        std::string netSysfsPath = "/sys/class/net/";
        std::ifstream operstate;
        struct stat buf;
        try {
            for (const auto& dir : fs::directory_iterator(netSysfsPath)) {
                if (dir.path().string() != "lo") {
                    operstate.open(dir.path().string() + "/operstate");
                    if (std::getline(operstate, operstateCont)) {
                        if (operstateCont == "up") {
                            wirelessPath = dir.path().string() + "/wireless";
                            if (stat(wirelessPath.c_str(), &buf) == 0)
                                formattedNetString += "  ";
                            else
                                formattedNetString += "  ";
                        }
                        operstate.close();
                    }
                }
            }
            formattedNetString.resize(formattedNetString.size() - 2);
        } catch (...) {
            // Couldn't iterate or no networks are connected; return empty string
            return "";
        }
        return formattedNetString;
    }

    void loop() {
        while (true) {
            printBuffer(formatNetworkString(), "net");
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }

    std::thread start() {
        std::thread t1(loop);
        return t1;
    }
}
