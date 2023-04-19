#include <Session.hpp>

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>

#include <openssl/sha.h>
#include <utility/base64.hpp>
#include <utility/StringConversion.hpp>

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Session::_globalSessionData;
std::mutex Session::_sessionMutex;
std::mt19937 Session::prng(std::chrono::system_clock::now().time_since_epoch().count());

Session::Session() : _wasSessionIDUpdated(false) {}

Session::~Session()
{
}

void Session::setSessionID(std::string newSessionID)
{
    _sessionID = newSessionID;
    validateSession();
}

std::string Session::needsToSendSessionID()
{
    if (_wasSessionIDUpdated)
    {
        _wasSessionIDUpdated = false;
        return _sessionID;
    }
    else
    {
        return "";
    }
}

void Session::extendSessionValidity()
{
    using namespace std::chrono;
    duration<int, std::ratio<60 * 30>> halfhour(1);
    system_clock::time_point now = time_point_cast<seconds>(system_clock::now()) + halfhour;
    _globalSessionData[_sessionID]["validUnit"] = StringUtils::toString(now);
}

void Session::generateSessionID()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t tt = system_clock::to_time_t(now);
    std::stringstream ss;
    ss << ctime(&tt);
    ss << prng();
    unsigned char obuf[20];
    std::string hash_this = ss.str();
    SHA1(reinterpret_cast<const unsigned char *>(hash_this.c_str()), hash_this.length(), obuf);
    _sessionID = base64_encode(obuf, 20);
    _globalSessionData[_sessionID] = std::unordered_map<std::string, std::string>();
    extendSessionValidity();
    _wasSessionIDUpdated = true;
}

void Session::validateSession()
{
    using namespace std::chrono;
    std::vector<std::string> toDelete;
    for (auto it : _globalSessionData)
    {
        using namespace std::chrono;
        system_clock::time_point now = time_point_cast<seconds>(system_clock::now());
        system_clock::time_point then = StringUtils::fromString<system_clock::time_point>(it.second["validUnit"]);
        if (now > then)
        {
            std::cout << "deleted a session: " << it.first << std::endl;
            toDelete.push_back(it.first);
        }
    }

    for (auto const &it : toDelete)
    {
        _globalSessionData.erase(it);
    }

    if (_globalSessionData.find(_sessionID) == _globalSessionData.end())
    {
        std::cout << "Could not find session in database\n";
        generateSessionID();
    }
    else
    {
        extendSessionValidity();
    }
}

void Session::set(const std::string &field, const std::string &value)
{
    std::lock_guard<std::mutex> guard(_sessionMutex);
    validateSession();
    _globalSessionData[_sessionID][field] = value;
}

int Session::get(const std::string &field, std::string &value)
{
    std::lock_guard<std::mutex> guard(_sessionMutex);
    validateSession();
    if (_globalSessionData[_sessionID].find(field) != _globalSessionData[_sessionID].end())
    {
        value = _globalSessionData[_sessionID][field];
        return 0;
    }
    std::cerr << "Field not found: " << field << "\n";
    return -1;
}

bool Session::has(const std::string &field)
{
    std::lock_guard<std::mutex> guard(_sessionMutex);
    validateSession();
    if (_globalSessionData[_sessionID].find(field) != _globalSessionData[_sessionID].end())
    {
        return true;
    }
    return false;
}