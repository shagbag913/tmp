#include <chrono>
#include <iostream>
#include <thread>
#include "main.h"
#include "module.h"

Module::Module(int sleepTime, std::string (*updateStatusFunction)(void), std::string moduleName) {
    this->sleepTime = sleepTime;
    this->updateStatusFunction = updateStatusFunction;
    this->moduleName = moduleName;
}

void Module::startLoop() {
    std::thread thread_obj(&Module::loop, this);
    thread_obj.join();
}

void Module::loop() {
    std::string newStatusString;
    while (true) {
        newStatusString = updateStatusFunction();
        if (newStatusString != statusString) {
            statusString = newStatusString;
            printBuffer(statusString, moduleName);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}
