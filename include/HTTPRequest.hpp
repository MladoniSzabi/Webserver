#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <unordered_map>

enum RequestMethod {GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, NOT_SPECIFIED};

struct HTTPRequest
{
    bool isWebsocket;

    RequestMethod requestMethod;
    std::string fileRequested;
    float httpVersion;

    std::unordered_map<std::string,std::string> fields;
    std::unordered_map<std::string,std::string> cookies;

    std::string body;
};

#endif