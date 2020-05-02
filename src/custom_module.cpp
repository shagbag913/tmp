#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include <stdio.h>

#include "config.h"
#include "custom_module.h"
#include "main.h"

std::vector<CustomModule> customModuleObjects;

void fillCustomModuleMap() {
    std::vector<std::string> customModuleNames;

    for (std::string configMatch : config::getMatchingConfigValues("custom_module_exec-"))
        customModuleNames.push_back(configMatch.substr(configMatch.find_first_of('-')+1,
                configMatch.find_first_of(' ')-1));

    for (std::string customModule : customModuleNames) {
        customModuleObjects.push_back(CustomModule(customModule,
                config::getConfigValue("custom_module_exec-" + customModule, ""),
                config::getConfigValue("custom_module_sleep-" + customModule, 0)));
    }
}

std::string exec(std::string command) {
   char buf[128];
   std::string result;

   FILE* pipe = popen(command.c_str(), "r");
   while (!feof(pipe)) {
      if (fgets(buf, sizeof(buf), pipe) != NULL)
         result += buf;
   }
   pclose(pipe);

   return result;
}

void customModuleLoop(CustomModule module) {
    while (true) {
        printBuffer(exec(module.exec), module.moduleName);
        std::this_thread::sleep_for(std::chrono::milliseconds(module.sleepTime));
    }
}

std::vector<std::thread> initializeCustomModuleThreads() {
    fillCustomModuleMap();

    std::vector<std::thread> customModuleThreads;

    for (CustomModule module : customModuleObjects) {
        customModuleThreads.push_back(std::thread(customModuleLoop, module));
    }

    return customModuleThreads;
}
