#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
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

std::vector<std::string> getModulesVector(std::string configString, std::string defaultValue) {
    std::string configModules = config::getConfigString(configString, defaultValue);

    if (configModules.find(",") != std::string::npos) {
        std::istringstream iter(configModules);
        std::string buf;
        std::vector<std::string> modules;
        while (getline(iter, buf, ',')) {
            modules.push_back(buf);
        }
        return modules;
    } else
        return std::vector<std::string>{configModules};
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

    std::vector<std::string> leftModules = getModulesVector("left_modules", "bspwm");
    std::vector<std::string> centerModules = getModulesVector("center_modules", "time,date");
    std::vector<std::string> rightModules = getModulesVector("right_modules", "backlight,net,battery");

    /* Configurable padding */
    int leftPadding = config::getConfigInt("left_padding", 0);
    int rightPadding = config::getConfigInt("right_padding", 0);

    std::string finalPrintBuffer;
    if (leftModules.size()) {
        finalPrintBuffer += "%{l}" + std::string(leftPadding, ' ');
        addToBuffer(leftModules, finalPrintBuffer);
    }

    if (centerModules.size()) {
        finalPrintBuffer += "%{c}";
        addToBuffer(centerModules, finalPrintBuffer);
    }

    if (rightModules.size()) {
        finalPrintBuffer += "%{r}";
        addToBuffer(rightModules, finalPrintBuffer);
        finalPrintBuffer += std::string(rightPadding, ' ');
    }

    std::cout << finalPrintBuffer << std::endl;
    printMutex.unlock();
}
