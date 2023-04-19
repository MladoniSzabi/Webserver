#ifndef SESSION_HPP
#define SESSION_HPP

#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <random>

class Session
{
public:
    Session();
    ~Session();

    int get(const std::string& field, std::string& value);
    void set(const std::string& field, const std::string& value);
    bool has(const std::string& field);
    std::string needsToSendSessionID();
    void setSessionID(std::string newSessionID);

private:

    void validateSession();
    void generateSessionID();
    void extendSessionValidity();

    std::string _sessionID;
    bool _wasSessionIDUpdated;
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _globalSessionData;
    static std::mutex _sessionMutex;
    static std::mt19937 prng;
    
};


#endif