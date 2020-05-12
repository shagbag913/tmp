#include <iostream>
#include <vector>

namespace config {
    void fillPropertyMap();
    void loop();
    std::string getConfigValue(const std::string&, const std::string&);
    int getConfigValue(const std::string&, int);
    std::vector<std::string> getMatchingConfigValues(const std::string&);
}
