#include <Client.hpp>

#include <iostream>
#include <Router.hpp>
#include <HTTPResponse.hpp>

Client::Client(ISocketClient *socket, std::function<void(Client *)> onClientFinished) : _socket(socket), _wasUpgradedToWebsocket(false)
{
    _requestParser.setRequestParsedCallback([=, this](HTTPRequest &r)
                                            { this->onRequestParsed(r); });
    _socket->setClientSendRequestCallback([=, this](char *buffer, int size)
                                          { if(size == -1) {
                                            onClientFinished(this);
                                            return;
                                          }
                                          this->_requestParser.add(buffer, size); });
    _socket->listen();
}

Client::~Client()
{
    if (_socket)
    {
        _socket->stop();
        delete _socket;
    }
}

void Client::onRequestParsed(HTTPRequest &request)
{
    Router r;
    HTTPResponse response;
    if (request.cookies.find("SessionID") != request.cookies.end())
    {
        _serversideSession.setSessionID(request.cookies["SessionID"]);
    }
    int retVal = r.request(response, request, this);
    if (retVal == -1)
    {
        request.fileRequested = "/error/404.html";
        if (r.request(response, request, this) == -1)
        {
            response.statusCode = 404;
            response.httpVersion = 1.1;
            response.fields["Server"] = "CinkerWebserver";
            response.fields["Connection"] = "keep-alive";
            response.fields["Content-Length"] = "0";
            response.body = "";
        }
        else
        {
            retVal = 0;
        }
    }
    std::string sID = _serversideSession.needsToSendSessionID();
    if (sID != "")
    {
        response.fields["Set-Cookie"] = "SessionID=" + sID;
    }
    response.getString();
    _socket->sendResponse(response.stringResponse);
}

Session &Client::getSession()
{
    return _serversideSession;
}

void Client::upgradeToWebsocket()
{
    std::cout << "Upgraded to websocket\n";
    _wasUpgradedToWebsocket = true;
}

ISocketClient *Client::getSocket()
{
    return _socket;
}