#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <interface/SocketServer.hpp>
#include <interface/SocketClient.hpp>
#include <Client.hpp>

#include <vector>
#include <semaphore>

class HTTPServer
{
public:
    HTTPServer(int port);
    ~HTTPServer();

    unsigned int getPort();
    bool isRunning();

private:
    void onClientDisconnected(Client *);
    void onClientConnected(ISocketClient *newClient);
    ISocketServer *_serverSocket;
    std::vector<Client *> _clients;

    std::binary_semaphore _sempahore{1};
};

#endif