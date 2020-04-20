#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include "battery.h"
#include "bspwm.h"

std::map<std::string, std::string> moduleMap;

/* Temp */
std::string defaultModuleOrder[] = {"battery", "bspwm"};

int main() {
    /* Start threads */
    std::thread t1 = bspwm::start();
    std::thread t2 = battery::start();

    t1.join();
    t2.join();
}

void printBuffer(std::string statusString, std::string moduleName) {
    std::string finalPrintBuffer;

    if (!moduleMap.count(moduleName))
        moduleMap.insert(std::pair<std::string, std::string>(moduleName, statusString));
    else
        moduleMap.at(moduleName) = statusString;

    for (std::string module : defaultModuleOrder)
        finalPrintBuffer += moduleMap[module] + "  |";

    /* Remove final seperator */
    finalPrintBuffer.resize(finalPrintBuffer.size() - 3);

    std::cout << finalPrintBuffer << std::endl;
}
