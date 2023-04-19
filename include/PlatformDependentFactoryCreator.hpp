#ifndef PLATFORM_DEPENDENT_FACTORY_HPP
#define PLATFORM_DEPENDENT_FACTORY_HPP

#include <interface/PlatformDependentFactoryInterface.hpp>

#ifdef __linux__
#include <linux/LinuxFactory.hpp>
#endif

class PlatformDependentFactoryCreator
{
public:
    PlatformDependentFactoryCreator()
    {
#ifdef __linux__
        _factory = new LinuxFactory();
#endif
    }

    ~PlatformDependentFactoryCreator()
    {
        delete _factory;
    }

    ISocketServer *getServerSocket(int port) { return _factory->getServerSocket(port); }
    ISocketClient *getClientSocket(NewConnection &conn) { return _factory->getClientSocket(conn); }

private:
    IPlatformDependentFactory *_factory;
};

#endif