#include <linux/LinuxSocketClient.hpp>
#include <linux/LinuxSocketDefinitions.hpp>

#include <fcntl.h>

#include <thread>
#include <string.h>
#include <iostream>

LinuxSocketClient::LinuxSocketClient(NewConnection &connInfo) : ISocketClient(connInfo)
{
    _socket = connInfo.socket;
    _ip = connInfo.addrData.sin_addr.s_addr;
    _isRunning = true;
    _buffer = new char[CLIENT_BUFFER_SIZE];
    fcntl(connInfo.socket, F_SETFL, O_NONBLOCK);
}

void LinuxSocketClient::setClientSendRequestCallback(std::function<void(char *, int)> cb)
{
    _onClientSendRequestCallback = cb;
}

LinuxSocketClient::~LinuxSocketClient()
{
    close(_socket);
    _isRunning = false;
    delete[] _buffer;
}

void LinuxSocketClient::listen()
{

    std::thread t = std::thread([=, this]
                                {
        unsigned int timeSinceUsed = 0;
        memset(_buffer, 0, CLIENT_BUFFER_SIZE);
        while(_isRunning) {
            int r = read( _socket , _buffer, CLIENT_BUFFER_SIZE);
            if(r != -1) {
                if(r == 0) {
                    _isRunning = false;
                }
                timeSinceUsed = 0;
                _onClientSendRequestCallback(_buffer, r);
                memset(_buffer, 0, CLIENT_BUFFER_SIZE);
            } else {
                if(errno == EAGAIN || errno == EWOULDBLOCK) {
                    timeSinceUsed += WAIT_TIME_PER_ITERATION;
                    if(timeSinceUsed >= MAX_WAIT_TIME) {
                        _isRunning = false;
                    } else {
                        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_PER_ITERATION));
                    }
                } else {
                    _isRunning = false;
                }
            }
        }
        _onClientSendRequestCallback(nullptr,-1); });
    t.detach();

    //_parser.addData(_buffer, r);
}

void LinuxSocketClient::sendResponse(std::string &data)
{
    int ptr = 0;
    while (ptr < data.length())
    {
        int i = send(_socket, data.c_str() + ptr, data.length() - ptr, 0);
        ptr += i;
    }
}

void LinuxSocketClient::stop()
{
    _isRunning = false;
}

uint32_t LinuxSocketClient::getIP()
{
    return _ip;
}