#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include "main.h"

namespace date {
    std::string formatDateString(tm *localTime) {
        std::string dateString = std::to_string(localTime->tm_mon+1);
        dateString += "/" + std::to_string(localTime->tm_mday);
        dateString += "/" + std::to_string(localTime->tm_year - 100);

        return dateString;
    }

    std::string formatTimeString(tm *localTime) {
        std::string timeString;

        /* 12 hour time */
        if (localTime->tm_hour == 0)
            timeString += "12";
        else if (localTime->tm_hour > 12)
            timeString += std::to_string(localTime->tm_hour-12);
        else
            timeString += std::to_string(localTime->tm_hour);

        timeString += ":";

        if (localTime->tm_min < 10)
            timeString += "0";
        timeString += std::to_string(localTime->tm_min);

        return timeString;
    }

    void loop() {
        time_t timeObj;
        tm *localTime;

        while (true) {
            timeObj = time(NULL);
            localTime = localtime(&timeObj);

            printBuffer(formatTimeString(localTime), "time");
            printBuffer(formatDateString(localTime), "date");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000*(60-localTime->tm_sec)));
        }
    }
}
