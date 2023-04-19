#ifndef PLATFORM_DEPENDENT_FACTORY_INTERFACE_HPP
#define PLATFORM_DEPENDENT_FACTORY_INTERFACE_HPP

#include "SocketServer.hpp"
#include "SocketClient.hpp"

class IPlatformDependentFactory
{
public:
    IPlatformDependentFactory() {}
    ~IPlatformDependentFactory() {}
    virtual ISocketServer *getServerSocket(int port)
    {
        std::cerr << "Platform dependent factory not implemented!" << std::endl;
        return nullptr;
    }
    virtual ISocketClient *getClientSocket(NewConnection &conn)
    {
        std::cerr << "Platform dependent factory not implemented!" << std::endl;
        return nullptr;
    }
};

#endif