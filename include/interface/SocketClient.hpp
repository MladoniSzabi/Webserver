#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP

#include <functional>
#include <string>
#include <iostream>

#include <netinet/ip.h>

struct NewConnection {
    NewConnection() {}
    NewConnection(int p_socket, sockaddr_in& p_addrData, int p_connInfoSize) : socket(p_socket), addrData(p_addrData), addrSize(p_connInfoSize) {}
    int socket;
    sockaddr_in addrData;
    int addrSize;
};

class ISocketClient {
public:
    ISocketClient(NewConnection&) {}
    virtual ~ISocketClient() {}

    virtual void stop() {}
    virtual void listen() {}
    virtual void sendResponse(std::string&) {}
    virtual uint32_t getIP() {return 0;}
    void sendWebsocketResponse(std::string& buffer) {
        std::string toSend = "";
        toSend += (char)0x81;
        if(buffer.length() < 126) {
            toSend += (char)buffer.length();
        } else {
            std::cout<<"Payload too big!\n";
            return;
        }
        toSend += buffer;
        sendResponse(toSend);
    }

    virtual void setClientSendRequestCallback(std::function<void(char*,int)>) {}
};

#endif