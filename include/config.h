#include <iostream>

namespace config {
    void fillPropertyMap();
    void loop();
    std::string getConfigValue(std::string prop, std::string defaultValue);
    int getConfigValue(std::string prop, int defaultValue);
}
