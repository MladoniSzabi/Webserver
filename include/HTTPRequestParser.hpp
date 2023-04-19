#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <sstream>
#include <functional>
#include <cstring>

#include <HTTPRequest.hpp>

struct tokens : std::ctype<char>
{
    tokens() : std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const *get_table()
    {
        typedef std::ctype<char> cctype;
        static const cctype::mask *const_rc = cctype::classic_table();

        static cctype::mask rc[cctype::table_size];
        memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

        rc[';'] = std::ctype_base::space;
        return &rc[0];
    }
};

class HTTPRequestParser
{
public:
    HTTPRequestParser();
    ~HTTPRequestParser();

    void setRequestParsedCallback(std::function<void(HTTPRequest &)>);
    void add(char *, int);

private:
    enum State
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        WEBSOCKET
    };

    void parse(std::stringstream &);

    void parseRequestLine(std::string &);
    void parseHeader(std::string &);
    void parseBody(std::string &);

    void addCookies(std::string &cookiesString);

    void setRequestMethod(std::string &);
    void setRequestFilename(std::string &);
    void setRequestHTTPVersion(std::string &);

    void parseWebsocketPacket(std::string line);

    void finishRequest();
    void finishFailedRequest();

    std::function<void(HTTPRequest &)> _onRequestParsedCallback;
    State _parsingState;
    HTTPRequest _ongoingRequest;
    int _bodyLength = -1;
};

#endif