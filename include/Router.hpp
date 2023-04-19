#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <Client.hpp>

#include <unordered_map>
#include <string>
#include <functional>

typedef std::function<void(HTTPResponse &, HTTPRequest &, Client *)> requestHandler;

class Router
{
public:
    Router();
    ~Router();

    void registerRoute(const std::string &, requestHandler);
    void registerFolderWithStaticFiles(const std::string &);
    int request(HTTPResponse &, HTTPRequest &, Client *);
    int serveStatic(HTTPResponse &, HTTPRequest &, Client *);
    int redirectToRoute(const std::string &, HTTPResponse &);
    HTTPResponse createRedirectResponse(const std::string &);

private:
    static std::unordered_map<std::string, requestHandler> _paths;
    static std::vector<std::string> _static_files_folder;
};

#endif