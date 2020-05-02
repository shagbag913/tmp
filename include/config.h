#include <iostream>
#include <vector>

namespace config {
    void fillPropertyMap();
    void loop();
    std::string getConfigValue(std::string prop, std::string defaultValue);
    int getConfigValue(std::string prop, int defaultValue);
    std::vector<std::string> getMatchingConfigValues(std::string);
}
