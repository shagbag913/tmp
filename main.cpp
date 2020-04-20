#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include "battery.h"

std::map<std::string, std::string> moduleMap;

int main() {
    /* Start threads */
    battery::start();
}

void printBuffer(std::string statusString, std::string moduleName) {
    if (!moduleMap.count(moduleName))
        moduleMap.insert(std::pair<std::string, std::string>(moduleName, statusString));
    else
        moduleMap.at(moduleName) = statusString;
    std::cout << moduleMap.at(moduleName) << std::endl;
}
