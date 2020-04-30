#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "main.h"
#include "utils.h"

namespace memory {

    int getMeminfo(std::string infoName) {
        std::ifstream meminfo("/proc/meminfo");
        std::string buf, currentInfoName, value;

        while (getline(meminfo, buf, '\n')) {
            currentInfoName = buf.substr(0, buf.find_last_of(':'));
            if (currentInfoName == infoName) {
                if (buf.find("kB") != std::string::npos) {
                    value = buf.substr(0, buf.size()-3);
                }
                value = value.substr(value.find_last_of(' ')+1, value.size());
            }
        }

        return std::stoi(value);
    }

    int getUsedMemory(int total, int available) {
        return utils::getPercentage(total - available /* Used memory */, total);
    }

    void loop() {
        int totalMemory = getMeminfo("MemTotal"), availableMemory;

        while (true) {
            availableMemory = getMeminfo("MemAvailable");
            printBuffer(" " + std::to_string(getUsedMemory(totalMemory, availableMemory)) + "%",
                    "memory");
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    }

    std::thread start() {
        std::thread t1(loop);
        return t1;
    }
}
