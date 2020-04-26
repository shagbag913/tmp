#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "config.h"

#include "modules/backlight.h"
#include "modules/battery.h"
#include "modules/bspwm.h"
#include "modules/date.h"
#include "modules/net.h"

std::map<std::string, std::string> moduleMap;

std::mutex printMutex;

/* Temp */
std::vector<std::string> defaultLeftModules{"bspwm"};
std::vector<std::string> defaultCenterModules{"time", "date"};
std::vector<std::string> defaultRightModules{"backlight", "net", "battery"};

int main() {
    /* Fill initial config map */
    config::fillPropertyMap();

    /* Start threads */
    std::thread t1 = bspwm::start();
    std::thread t2 = battery::start();
    std::thread t3 = date::start();
    std::thread t4 = net::start();
    std::thread t5 = backlight::start();
    std::thread t6 = config::start();

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}

void addToBuffer(std::vector<std::string>& modules, std::string& buffer) {
    std::string seperator = "  |  ";

    for (std::string module : modules)
        if (!moduleMap[module].empty())
            buffer += moduleMap[module] + seperator;

    if (buffer.size() > seperator.size())
        buffer.resize(buffer.size() - seperator.size());
}

void printBuffer(std::string statusString, std::string moduleName) {
    printMutex.lock();
    if (!moduleMap.count(moduleName))
        moduleMap.insert(std::pair<std::string, std::string>(moduleName, statusString));
    else
        if (statusString.empty())
            moduleMap.erase(moduleName);
        else
            moduleMap.at(moduleName) = statusString;

    std::string finalPrintBuffer = "%{l}";
    addToBuffer(defaultLeftModules, finalPrintBuffer);

    finalPrintBuffer += "%{c}";
    addToBuffer(defaultCenterModules, finalPrintBuffer);

    finalPrintBuffer += "%{r}";
    addToBuffer(defaultRightModules, finalPrintBuffer);

    std::cout << finalPrintBuffer << std::endl;
    printMutex.unlock();
}
