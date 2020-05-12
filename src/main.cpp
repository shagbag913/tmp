#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "config.h"
#include "custom_module.h"
#include "module_base.h"

#include "modules/backlight.h"
#include "modules/battery.h"
#include "modules/bspwm.h"
#include "modules/date.h"
#include "modules/memory.h"
#include "modules/net.h"

std::map<std::string, std::string> moduleMap;

std::mutex printMutex;

int main() {
    /* Fill initial config map */
    config::fillPropertyMap();

    Module battery = Module(battery::loop);
    Module bspwm = Module(bspwm::loop);
    Module date = Module(date::loop);
    Module net = Module(net::loop);
    Module backlight = Module(backlight::loop);
    Module memory = Module(memory::loop);

    battery.startLoop();
    bspwm.startLoop();
    date.startLoop();
    net.startLoop();
    backlight.startLoop();
    memory.startLoop();

    /* Start custom module threads */
    std::vector<std::thread> customModuleThreads = initializeCustomModuleThreads();

    /* Start config checker loop thread */
    std::thread configThread = std::thread(config::loop);
    configThread.join();
}

void addToBuffer(const std::vector<std::string>& modules, std::string& buffer) {
    std::string seperator = "  " + config::getConfigValue("module_seperator", "|") + "  ";

    for (std::string module : modules)
        if (!moduleMap[module].empty())
            buffer += moduleMap[module] + seperator;

    if (buffer.size() > seperator.size())
        buffer.resize(buffer.size() - (seperator.size()-1));
}

std::vector<std::string> getModulesVector(const std::string& configString,
        const std::string& defaultValue) {
    std::string configModules = config::getConfigValue(configString, defaultValue);

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

void printBuffer(const std::string& statusString, const std::string& moduleName) {
    printMutex.lock();
    if (!moduleMap.count(moduleName))
        moduleMap.insert(std::pair<std::string, std::string>(moduleName, statusString));
    else
        if (statusString.empty())
            moduleMap.erase(moduleName);
        else
            if (moduleMap[moduleName] != statusString)
                moduleMap[moduleName] = statusString;
            else {
                printMutex.unlock();
                return;
            }

    std::vector<std::string> leftModules = getModulesVector("left_modules", "bspwm");
    std::vector<std::string> centerModules = getModulesVector("center_modules", "time,date");
    std::vector<std::string> rightModules = getModulesVector("right_modules", "memory,backlight,net,battery");

    /* Configurable padding */
    int leftPadding = config::getConfigValue("left_padding", 0);
    int rightPadding = config::getConfigValue("right_padding", 0);

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
