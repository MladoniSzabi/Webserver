#ifndef LINUX_FACTORY_HPP
#define LINUX_FACTORY_HPP

#include <interface/PlatformDependentFactoryInterface.hpp>
#include <linux/LinuxSocketServer.hpp>
#include <linux/LinuxSocketClient.hpp>

class LinuxFactory : public IPlatformDependentFactory
{
public:
    LinuxFactory() {}
    ~LinuxFactory() {}

    ISocketServer *getServerSocket(int port) { return new LinuxSocketServer(port); }
    ISocketClient *getClientSocket(NewConnection &conn) { return new LinuxSocketClient(conn); }
};

#endif