#include <iostream>
#include <thread>
#include <vector>

class CustomModule {
    public:
        CustomModule(std::string moduleName, std::string exec, int sleepTime) {
            this->moduleName = moduleName;
            this->exec = exec;
            this->sleepTime = sleepTime;
        }

        std::string moduleName;
        std::string exec;
        int sleepTime;
};

std::vector<std::thread> initializeCustomModuleThreads();
