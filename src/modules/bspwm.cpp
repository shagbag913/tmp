#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/un.h>
#include <map>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "main.h"

namespace bspwm {

    /* Workspace index: workspace ID */
    std::map<int, std::string> workspaceMap;

    /* Workspace ID: workspace has nodes */
    std::map<std::string, bool> nodeMap;

    int openBspwmSocket() {
        /* bspwm supports setting socket via BSPWM_SOCKET env var */
        const char *socketPath = std::getenv("BSPWM_SOCKET");
        if (socketPath == nullptr)
            socketPath = "/tmp/bspwm_0_0-socket";

        struct sockaddr_un addr = { AF_UNIX };
        strcpy(addr.sun_path, socketPath);

        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (!sock)
            return 0;
        if (connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0)
            return 0;

        return sock;
    }

    bool writeToSocket(int sock, const char *msg) {
        char *formattedMsg = new char[strlen(msg)+1];
        for (int x = 0; x < (int)strlen(msg); x++) {
            if (msg[x] == ' ')
                std::sprintf(formattedMsg+x, "%c", 0);
            else
                std::sprintf(formattedMsg+x, "%c", msg[x]);
        }
        bool writeSuccessful = send(sock, formattedMsg, strlen(msg)+1, 0) != -1;
        delete[] formattedMsg;
        return writeSuccessful;
    }

    std::string readSocketOutput(int sock) {
        std::string output(1024, 0);
        int numRead = read(sock, &output[0], 1023);
        if (numRead > 1)
            output.resize(numRead-1);
        else
            return "";
        return output;
    }

    void fillWorkspaceMap(int sock) {
        writeToSocket(sock, "query --desktops");
        std::istringstream workspaces(readSocketOutput(sock));
        std::string buf;
        int iter = 0;

        while (std::getline(workspaces, buf, '\n')) {
            workspaceMap.insert(std::pair<int, std::string>(iter, buf));
            iter++;
        }
    }

    std::string filterBetweenNthOccurence(int occurence, std::string strn) {
        int n = 0, x;
        for (x = 0; x < (int)strn.size(); x++) {
            if (strn[x] == ' ') {
                n++;
                if (n == occurence)
                    break;
            }
        }
        return strn.substr(x+1, 10);
    }

    std::string filterDesktopId(std::string strn) {
        return filterBetweenNthOccurence(2, strn);
    }

    void subscribeToWMEvents(int sock) {
        writeToSocket(sock, "subscribe desktop_focus node_add node_remove node_transfer");
    }

    bool doesDesktopHaveNodes(int& sock, std::string desktopId) {
        char *queryMsg = new char[sizeof("query --nodes --desktop ") + desktopId.size() + 1];
        sprintf(queryMsg, "query --nodes --desktop %s", desktopId.c_str());

        close(sock);
        sock = openBspwmSocket();

        writeToSocket(sock, queryMsg);

        delete[] queryMsg;

        std::string output = readSocketOutput(sock);

        /* Resubscribe to wm events */
        sock = openBspwmSocket();
        subscribeToWMEvents(sock);

        return !output.empty();
    }

    std::string formatBspwmWorkspaceStatus(int& sock, std::string newFocus) {
        std::string formattedWorkspaceStatus;

        for (std::map<int, std::string>::iterator x = workspaceMap.begin();
                x != workspaceMap.end(); x++) {
            std::string wsIndxString = std::to_string(x->first+1);
            if (newFocus != x->second && doesDesktopHaveNodes(sock, x->second))
                formattedWorkspaceStatus += "%{A:bspc desktop -f " + x->second + ":}  "
                        + wsIndxString + "  %{A} | ";
            else if (newFocus == x->second)
                formattedWorkspaceStatus += "%{+u}  " + wsIndxString + "  %{-u} | ";
        }

        /* No need for a size check here, aleady checked in the loop. */
        formattedWorkspaceStatus.resize(formattedWorkspaceStatus.size() - 3);
        return formattedWorkspaceStatus;
    }

    void startLoop() {
        int sock = openBspwmSocket();

        /* Fill initial map */
        fillWorkspaceMap(sock);

        std::string focusedWorkspace;

        while (true) {
            if (focusedWorkspace.empty()) {
                /* For the first loop, get the currently focused desktop */
                sock = openBspwmSocket();
                writeToSocket(sock, "query --desktop --desktops");
                focusedWorkspace = readSocketOutput(sock);

                /* Now subscribe to focus changes */
                sock = openBspwmSocket();
                subscribeToWMEvents(sock);
            } else {
                /* Filter focus events and update focusedWorkspace */
                std::istringstream socketOutput(readSocketOutput(sock));
                std::string buf;
                while (std::getline(socketOutput, buf, '\n')) {
                    if (buf.find("desktop_focus") != std::string::npos) {
                        focusedWorkspace = filterDesktopId(buf);
                        break;
                    }
                }
            }

            /* If focusedWorkspace is empty, bspwm socket wasn't able to be read.
             * TODO: handle this better in the future, maybe reconnect to the socket in this case?
             */
            if (!focusedWorkspace.empty())
                printBuffer(formatBspwmWorkspaceStatus(sock, focusedWorkspace), "bspwm");
        }
    }

    std::thread start() {
        std::thread t1(startLoop);
        return t1;
    }
}
