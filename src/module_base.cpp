#include <iostream>
#include <thread>

#include "module_base.h"

Module::Module(void (*loopFunction)()) {
    this->loopFunction = loopFunction;
}

void Module::startLoop() {
    loopThread = std::thread(loopFunction);
}

void Module::stopLoop() {
    loopThread = std::thread();
}
