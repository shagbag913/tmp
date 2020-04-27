#include <iostream>
#include <thread>

namespace config {
    void fillPropertyMap();
    std::thread start();
    std::string getConfigValue(std::string prop, std::string defaultValue);
    int getConfigValue(std::string prop, int defaultValue);
}
