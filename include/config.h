#include <iostream>
#include <thread>

namespace config {
    void fillPropertyMap();
    std::thread start();
    std::string getConfigString(std::string prop, std::string defaultValue);
}
