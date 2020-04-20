#include "module.h"
#include <chrono>
#include <thread>

Module::Module(int sleepTime, void (*loopFunction)(void)) {
    this->sleepTime = sleepTime;
    this->loopFunction = loopFunction;
    startLoop();
}

void Module::startLoop() {
    while (true) {
        loopFunction();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}
