#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <interface/SocketClient.hpp>
#include <HTTPRequestParser.hpp>
#include <Session.hpp>

class Client
{
public:
    Client(ISocketClient *, std::function<void(Client *)>);
    ~Client();

    void onRequestParsed(HTTPRequest &);
    void upgradeToWebsocket();
    ISocketClient *getSocket();

    Session &getSession();

private:
    ISocketClient *_socket;
    HTTPRequestParser _requestParser;
    Session _serversideSession;
    bool _wasUpgradedToWebsocket;
};

#endif