#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <thread>

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
            std::string prop = formatProperty(buf);
            std::string value = formatValue(buf);
            propMap.insert(std::pair<std::string, std::string>(prop, value));
        }
    }

    void startLoop() {
        while (true) {
            fillPropertyMap();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    std::thread start() {
        std::thread t1(startLoop);
        return t1;
    }

    std::string getConfigString(std::string prop, std::string defaultValue) {
        if (propMap.count(prop))
            return propMap.at(prop);

        return defaultValue;
    }
}
