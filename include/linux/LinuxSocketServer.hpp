#ifndef LINUX_SOCKET_SERVER_HPP
#define LINUX_SOCKET_SERVER_HPP

#include <interface/SocketServer.hpp>

class LinuxSocketServer : public ISocketServer
{
public:
    LinuxSocketServer(int port);
    ~LinuxSocketServer();

    void setClientConnectedCallback(onClientConnectedCallback cb);
    void startListening();
    void stop();
    unsigned int getPort();
    bool isRunning();

private:
    int _serverSocket;
    onClientConnectedCallback _onClientConnectedCallback;
    bool _isRunning;
};

#endif