#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <HTTPStatusPhrases.hpp>

struct HTTPResponse
{
    float httpVersion = 1.1;
    int statusCode = 200;
    std::unordered_map<std::string, std::string> fields;

    std::string body;

    std::string stringResponse;

    void getString()
    {
        if (fields.find("Content-Length") == fields.end() && body.size() > 0)
        {
            fields["Content-Length"] = std::to_string(body.size());
        }
        std::stringstream ss;
        ss << std::setprecision(2);
        ss << std::noshowpoint;
        ss << "HTTP/";
        ss << httpVersion << " ";
        ss << statusCode << " ";
        ss << statusCodeToPhrase.find(statusCode)->second + "\r\n";
        httpVersion = 0;
        statusCode = 0;
        for (auto it : fields)
        {
            ss << it.first << ": " << it.second + "\r\n";
        }
        fields.clear();
        ss << "\r\n";
        ss << body;
        stringResponse = ss.str();
        body = "";
    }
};

#endif