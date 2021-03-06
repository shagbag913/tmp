#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>

namespace config {

    std::map<std::string, std::string> propMap;

    std::ifstream openConfigFile() {
        const char *xdgHome = std::getenv("XDG_CONFIG_HOME");
        std::string configPath;
        if (xdgHome == nullptr) {
            xdgHome = std::getenv("HOME");
            configPath = std::string(xdgHome) + "/.config/shagbar/config";
        } else
            configPath = std::string(xdgHome) + "/shagbar/config";

        struct stat buf;
        if (stat(configPath.c_str(), &buf) != 0)
            return std::ifstream(nullptr);

        return std::ifstream(configPath);
    }

    std::string formatProperty(std::string line) {
        return line.substr(0, line.find_first_of("=")-1);
    }

    std::string formatValue(std::string line) {
        return line.substr(line.find_first_of("=")+2, line.size());
    }

    void fillPropertyMap() {
        std::ifstream configFile = openConfigFile();
        if (!configFile.is_open())
            return;

        propMap.clear();

        std::string buf;
        while (std::getline(configFile, buf, '\n')) {
            if (!buf.size() || buf[0] == '#')
                continue;
            std::string prop = formatProperty(buf);
            std::string value = formatValue(buf);
            propMap.insert(std::pair<std::string, std::string>(prop, value));
        }
    }

    void loop() {
        while (true) {
            fillPropertyMap();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    std::string getConfigValue(const std::string& prop, const std::string& defaultValue) {
        if (propMap.count(prop))
            return propMap.at(prop);

        return defaultValue;
    }

    int getConfigValue(const std::string& prop, int defaultValue) {
        return std::stoi(getConfigValue(prop, std::to_string(defaultValue)));
    }

    std::vector<std::string> getMatchingConfigValues(const std::string& prop) {
        std::vector<std::string> matchingConfigs;
        for (std::map<std::string, std::string>::iterator x = propMap.begin(); x != propMap.end();
                ++x)
            if (x->first.find(prop) != std::string::npos)
                matchingConfigs.push_back(x->first);
        return matchingConfigs;
    }
}
