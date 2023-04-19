#include <HTTPRequestParser.hpp>

#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iterator>
#include <stdint.h>

HTTPRequestParser::HTTPRequestParser() : _parsingState(State::REQUEST_LINE)
{
    _ongoingRequest.isWebsocket = false;
}
HTTPRequestParser::~HTTPRequestParser() {}

void HTTPRequestParser::setRequestParsedCallback(std::function<void(HTTPRequest &)> cb)
{
    _onRequestParsedCallback = cb;
}

void HTTPRequestParser::add(char *buffer, int size)
{
    std::string s(buffer, size);
    std::stringstream ssbuffer;
    ssbuffer << s;
    parse(ssbuffer);
}

void HTTPRequestParser::parse(std::stringstream &buffer)
{
    std::string s;
    bool shouldExitLoop = false;
    while (std::getline(buffer, s) && !shouldExitLoop)
    {
        switch (_parsingState)
        {
        case State::REQUEST_LINE:
            parseRequestLine(s);
            if (_ongoingRequest.isWebsocket)
            {
                parseWebsocketPacket(buffer.str());
                shouldExitLoop = true;
            }
            break;

        case State::HEADERS:
            parseHeader(s);
            break;

        case State::BODY:
            parseBody(s);
            break;
        case State::WEBSOCKET:
            parseWebsocketPacket(buffer.str());
            shouldExitLoop = true;
            break;
        }
    }
}

/*GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE*/

void HTTPRequestParser::parseRequestLine(std::string &line)
{
    std::stringstream ss;
    ss << line;
    std::string requestMethod, filename, httpVersion;
    ss >> requestMethod;
    ss >> filename;
    ss >> httpVersion;
    setRequestMethod(requestMethod);
    if (_ongoingRequest.requestMethod == RequestMethod::NOT_SPECIFIED)
    {
        _ongoingRequest.isWebsocket = true;
        _parsingState = State::WEBSOCKET;
        return;
    }
    setRequestFilename(filename);
    setRequestHTTPVersion(httpVersion);
    _parsingState = State::HEADERS;
}

void HTTPRequestParser::setRequestMethod(std::string &method)
{
    std::transform(method.begin(), method.end(), method.begin(), ::tolower);
    if (method == "get")
        _ongoingRequest.requestMethod = RequestMethod::GET;
    else if (method == "head")
        _ongoingRequest.requestMethod = RequestMethod::HEAD;
    else if (method == "post")
        _ongoingRequest.requestMethod = RequestMethod::POST;
    else if (method == "put")
        _ongoingRequest.requestMethod = RequestMethod::PUT;
    else if (method == "delete")
        _ongoingRequest.requestMethod = RequestMethod::DELETE;
    else if (method == "connect")
        _ongoingRequest.requestMethod = RequestMethod::CONNECT;
    else if (method == "options")
        _ongoingRequest.requestMethod == RequestMethod::OPTIONS;
    else if (method == "trace")
        _ongoingRequest.requestMethod == RequestMethod::TRACE;
    else
        _ongoingRequest.requestMethod = RequestMethod::NOT_SPECIFIED;
}

void HTTPRequestParser::setRequestFilename(std::string &filename)
{
    _ongoingRequest.fileRequested = filename;
}

void HTTPRequestParser::setRequestHTTPVersion(std::string &version)
{
    std::string s_versionNumber = "";
    for (unsigned int i = 5; i < version.length(); i++)
    {
        s_versionNumber += version[i];
    }
    float f_versionNumber = std::stof(s_versionNumber);
    if (f_versionNumber != 0.9 && f_versionNumber != 1.0 && f_versionNumber != 1.1 && int(f_versionNumber) != f_versionNumber)
    {
        f_versionNumber = 0;
    }
    _ongoingRequest.httpVersion = f_versionNumber;
}

void HTTPRequestParser::parseHeader(std::string &line)
{
    line.pop_back();
    if (line == "")
    {
        if (_ongoingRequest.requestMethod == RequestMethod::GET)
        {
            finishRequest();
            return;
        }
        _parsingState = State::BODY;
        return;
    }
    std::stringstream ss;
    ss << line;
    std::string field, value;
    if (getline(ss, field, ':') && getline(ss, value))
    {
        if (field == "Cookie")
        {
            addCookies(value);
        }
        else if (field == "Content-Length")
        {
            _bodyLength = std::stoi(value);
        }
        else
        {
            _ongoingRequest.fields[field] = value.substr(1);
        }
    }
}

void HTTPRequestParser::addCookies(std::string &cookiesString)
{
    std::stringstream ss(cookiesString);
    ss.imbue(std::locale(std::locale(), new tokens()));
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> cookies(begin, end);
    for (unsigned int i = 0; i < cookies.size(); i++)
    {
        std::string field, value;
        int splittingPoint = cookies[i].find("=");
        field = cookies[i].substr(0, splittingPoint);
        value = cookies[i].substr(splittingPoint + 1);
        _ongoingRequest.cookies[field] = value;
    }
}

void HTTPRequestParser::parseBody(std::string &line)
{
    if (_bodyLength == -1)
    {
        _ongoingRequest.body += line;
        if (line.substr(line.size() - 2) == "\n\r")
        {
            finishRequest();
        }
    }
    else
    {
        _ongoingRequest.body += line;
        _bodyLength -= line.size();
        if (_bodyLength <= 0)
        {
            finishRequest();
        }
    }
}

void HTTPRequestParser::finishRequest()
{
    _onRequestParsedCallback(_ongoingRequest);
    _ongoingRequest = HTTPRequest();
    _parsingState = State::REQUEST_LINE;
    _ongoingRequest.isWebsocket = false;
}

void HTTPRequestParser::finishFailedRequest()
{
    _ongoingRequest = HTTPRequest();
    _parsingState = State::REQUEST_LINE;
    _ongoingRequest.isWebsocket = false;
}

/// Websocket frame representaion
///   0                   1                   2                   3
///   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
///  +-+-+-+-+-------+-+-------------+-------------------------------+
///  |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
///  |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
///  |N|V|V|V|       |S|             |   (if payload len==126/127)   |
///  | |1|2|3|       |K|             |                               |
///  +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
///  |     Extended payload length continued, if payload len == 127  |
///  + - - - - - - - - - - - - - - - +-------------------------------+
///  |                               |Masking-key, if MASK set to 1  |
///  +-------------------------------+-------------------------------+
///  | Masking-key (continued)       |          Payload Data         |
///  +-------------------------------- - - - - - - - - - - - - - - - +
///  :                     Payload Data continued ...                :
///  + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
///  |                     Payload Data continued ...                |
///  +---------------------------------------------------------------+
/// link to spec: https://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17#section-5

void HTTPRequestParser::parseWebsocketPacket(std::string line)
{
    const char *buffer = line.c_str();
    unsigned int offset = 0;
    /// first bit
    bool isLastPacket = ((uint8_t)(*buffer) & 0x80) >> 7;

    /// bits 1,2,3
    uint8_t rsv = ((uint8_t)(*buffer) & 0x70) >> 4;
    if (rsv != 0)
    {
        std::cout << "RSV flags should be 0!!\n";
        finishFailedRequest();
        return;
    }

    /// bits 4, 5, 6, 7
    uint8_t opcode = ((uint8_t)(*buffer) & 0x0f);
    offset++;

    /// bit 8
    bool isMasked = ((uint8_t)(*(buffer + offset)) & 0x80) >> 7;

    /// small length
    uint64_t payloadLength = ((uint8_t) * (buffer + offset) & 0x7f);
    offset++;

    /// extended length
    if (payloadLength == 0x7e)
    {
        payloadLength = (uint8_t) * (buffer + offset);
        offset++;
        payloadLength <<= 8;
        payloadLength += (uint8_t) * (buffer + offset);
        offset++;
    }

    /// extended length continued
    if (payloadLength == 0x7f)
    {
        for (int i = 0; i < 6; i++)
        {
            payloadLength <<= 8;
            payloadLength += (uint8_t) * (buffer + offset);
            offset++;
        }
    }

    /// masking key
    uint8_t maskingKey[4];
    if (isMasked)
    {
        for (int i = 0; i < 4; i++)
        {
            maskingKey[i] = (uint8_t) * (buffer + offset);
            offset++;
        }
    }

    /// payload
    for (uint64_t i = 0; i < payloadLength; i++)
    {
        uint8_t data = (uint8_t) * (buffer + offset + i);
        if (isMasked)
        {
            data ^= maskingKey[i % 4];
        }
        _ongoingRequest.body += (char)data;
    }

    /// The End
    if (isLastPacket)
    {
        finishRequest();
    }
}