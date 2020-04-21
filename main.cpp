#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include "backlight.h"
#include "battery.h"
#include "bspwm.h"
#include "date.h"
#include "net.h"

std::map<std::string, std::string> moduleMap;

/* Temp */
std::string defaultLeftModules[] = {"bspwm"};
std::string defaultCenterModules[] = {"time", "date"};
std::string defaultRightModules[] = {"backlight", "net", "battery"};

int main() {
    /* Start threads */
    std::thread t1 = bspwm::start();
    std::thread t2 = battery::start();
    std::thread t3 = date::start();
    std::thread t4 = net::start();
    std::thread t5 = backlight::start();

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
}

void printBuffer(std::string statusString, std::string moduleName) {
    if (!moduleMap.count(moduleName))
        moduleMap.insert(std::pair<std::string, std::string>(moduleName, statusString));
    else
        moduleMap.at(moduleName) = statusString;

    std::string finalPrintBuffer = "%{l}";
    for (std::string module : defaultLeftModules)
        finalPrintBuffer += moduleMap[module] + "  |  ";

    finalPrintBuffer.resize(finalPrintBuffer.size() - 5);
    finalPrintBuffer += "%{c}";

    for (std::string module : defaultCenterModules)
        finalPrintBuffer += moduleMap[module] + "  |  ";

    finalPrintBuffer.resize(finalPrintBuffer.size() - 5);
    finalPrintBuffer += "%{r}";

    for (std::string module : defaultRightModules)
        finalPrintBuffer += moduleMap[module] + "  |  ";

    /* Remove final seperator */
    finalPrintBuffer.resize(finalPrintBuffer.size() - 5);

    std::cout << finalPrintBuffer << std::endl;
}
