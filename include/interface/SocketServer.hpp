#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <netinet/in.h>
#include <interface/SocketClient.hpp>

#include <functional>

typedef std::function<void(ISocketClient *)> onClientConnectedCallback;

/// Inherit from this to create platform specific servers.
class ISocketServer
{
public:
    ISocketServer(int port) {}
    virtual ~ISocketServer() {}

    virtual void setClientConnectedCallback(onClientConnectedCallback cb) {}
    virtual void startListening() {}
    virtual void stop() {}

    virtual unsigned int getPort()
    {
        std::cerr << "Socket Server not implemented!" << std::endl;
        return 0;
    }

    virtual bool isRunning()
    {
        std::cerr << "Socket Server not implemented!" << std::endl;
        return false;
    }
};

#endif