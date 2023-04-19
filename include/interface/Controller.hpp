#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <Router.hpp>
#include <HTTPResponse.hpp>
#include <interface/FileParser.hpp>

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>

class IController {
public:
    IController() {}
    virtual ~IController() {}

    static void addRoute(std::string path, requestHandler cb) {
        Router r;
        r.registerRoute(path, cb);
    }

    static void render(HTTPResponse& r, IFileParser* parser) {

        r.body = parser->parse();
        r.body += "\r\n";

        r.statusCode = 200;
        r.httpVersion = 1.1;

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        char date[80];
        strftime(date, 80, "%a,%e %b %T GMT", now);

        r.fields["Date"] = std::string(date);
        r.fields["Server"] = "CinkerWebserver";
        r.fields["Content-type"] = "text/html";
        r.fields["Content-Length"] = std::to_string(r.body.length());
        r.fields["Connection"] = "keep-alive";
        delete parser;
    }
};

#endif