#ifndef LINUX_SOCKET_CLIENT_HPP
#define LINUX_SOCKET_CLIENT_HPP

#include <interface/SocketClient.hpp>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>

#include <string>

const int CLIENT_BUFFER_SIZE = 2048;

class LinuxSocketClient : public ISocketClient {
public:
    LinuxSocketClient(NewConnection& connInfo);
    ~LinuxSocketClient();

    void setClientSendRequestCallback(std::function<void(char*, int)>);

    void stop();
    void sendResponse(std::string&);
    uint32_t getIP();

private:

    void listen();

    std::string _address;
    uint32_t _ip;
    std::function<void(char*, int)> _onClientSendRequestCallback;
    int _socket;
    char* _buffer;
    bool _isRunning;
};

#endif