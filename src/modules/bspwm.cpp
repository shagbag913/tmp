#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/un.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "main.h"

namespace bspwm {

    int subscribeToEvents() {
        /* bspwm supports setting socket via BSPWM_SOCKET env var */
        const char *socketPath = std::getenv("BSPWM_SOCKET");
        if (socketPath == nullptr)
            socketPath = "/tmp/bspwm_0_0-socket";

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, socketPath);

        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (!sock)
            return 0;
        if (connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0)
            return 0;
        if (send(sock, "subscribe", sizeof("subscribe"), 0) < 0)
            return 0;

        return sock;
    }

    void printBspwmBuffer(std::string wmStatus) {
        std::istringstream socketStatus(wmStatus);
        std::string ws, formattedStatus, wsIndxString;
        int wsIndx = 0;
        while (std::getline(socketStatus, ws, ':')) {
            if (ws == "LT" || ws == "LM")
                break;

            wsIndxString = std::to_string(wsIndx);

            if (ws[0] == 'O' || ws[0] == 'F')
                formattedStatus += "%{+u}  " + wsIndxString + "  %{-u} | ";
            else if (ws[0] == 'o')
                formattedStatus += "%{A:bspc desktop -f ^" + wsIndxString + ":}  " + wsIndxString + "  %{A} | ";

            wsIndx++;
        }

        /* Remove final seperator */
        formattedStatus.resize(formattedStatus.size() - 3);

        printBuffer(formattedStatus, "bspwm");
    }

    void startLoop() {
        std::string wmStatus(1024, 0);
        int sock = subscribeToEvents();

        while (true) {
            if (read(sock, &wmStatus[0], 1023) > -1)
                printBspwmBuffer(wmStatus);
        }
    }

    std::thread start() {
        std::thread t1(startLoop);
        return t1;
    }
}
