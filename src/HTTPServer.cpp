#include <HTTPServer.hpp>

#include <iostream>

#include <PlatformDependentFactoryCreator.hpp>

HTTPServer::HTTPServer(int port)
{
    auto pdfc = PlatformDependentFactoryCreator();
    _serverSocket = pdfc.getServerSocket(port);
    _serverSocket->setClientConnectedCallback([=, this](ISocketClient *client)
                                              { this->onClientConnected(client); });
    _serverSocket->startListening();
}

HTTPServer::~HTTPServer()
{
    for (auto i : _clients)
    {
        delete i;
    }
    delete _serverSocket;
}

void HTTPServer::onClientConnected(ISocketClient *newClientSocket)
{
    Client *c = new Client(newClientSocket, [=, this](Client *deadClient)
                           { this->onClientDisconnected(deadClient); });
    _sempahore.acquire();
    _clients.push_back(c);
    _sempahore.release();
}

void HTTPServer::onClientDisconnected(Client *client)
{
    _sempahore.acquire();

    // Find client to delete (disconnected)
    unsigned int i = 0;
    unsigned int clientIndex = -1;
    for (; i < _clients.size(); i++)
    {
        if (_clients[i] == client)
        {
            clientIndex = i;
            break;
        }
    }

    // Attempt to delete client
    if (clientIndex != -1)
    {
        delete _clients[clientIndex];
        _clients.erase(_clients.begin() + clientIndex);
    }
    else
    {
        std::cout << "ERROR: Attempting to delete a client that does not exist" << std::endl;
    }
    _sempahore.release();
}

unsigned int HTTPServer::getPort()
{
    return _serverSocket->getPort();
}

bool HTTPServer::isRunning()
{
    return _serverSocket;
}