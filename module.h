#include <iostream>

class Module {
public:
    Module(int sleepTime, std::string (*updateStatusFunction)(void), std::string moduleName);
    int sleepTime;
    std::string (*updateStatusFunction)(void);
    void startLoop();
    std::string statusString;
    std::string moduleName;
private:
    void loop();
};
