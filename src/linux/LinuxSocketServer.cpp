#include <linux/LinuxSocketServer.hpp>

#include <linux/LinuxSocketClient.hpp>
#include <linux/LinuxSocketDefinitions.hpp>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include <thread>

LinuxSocketServer::LinuxSocketServer(int port) : ISocketServer(port), _isRunning(false), _onClientConnectedCallback(nullptr)
{
    struct sockaddr_in addr;
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1)
    {
        std::cerr << "Error opening socket!" << std::endl;
        _exit(-1);
    }

    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    if (bind(_serverSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        std::cerr << "Error binding socket!" << std::endl;
        _exit(-1);
    }
    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
    std::cout << addr.sin_port << std::endl;
}

LinuxSocketServer::~LinuxSocketServer()
{
    _isRunning = false;
    close(_serverSocket);
}

void LinuxSocketServer::startListening()
{

    std::thread t = std::thread([=, this]
                                {
        if(listen(this->_serverSocket, 10) == -1) {
            std::cerr<<"The server encountered an error while listening for connections"<<std::endl;
            return;
        }
        this->_isRunning = true;
        while (this->_isRunning)
        {
            struct sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(this->_serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
            if (clientSocket != -1 && this->_onClientConnectedCallback)
            {
                NewConnection connInfo(clientSocket, clientAddr, clientAddrLen);
                ISocketClient *newClient = new LinuxSocketClient(connInfo);
                this->_onClientConnectedCallback(newClient);
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_PER_ITERATION));
                }
                else
                {
                    std::cerr << "The server encountered an error while accepting for connections" << std::endl;
                    _isRunning = false;
                }
            }
        } });
    t.detach();
}

void LinuxSocketServer::setClientConnectedCallback(onClientConnectedCallback cb)
{
    _onClientConnectedCallback = cb;
}

void LinuxSocketServer::stop()
{
    _isRunning = false;
}

unsigned int LinuxSocketServer::getPort()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    if (getsockname(this->_serverSocket, (struct sockaddr *)&addr, &len) == -1)
    {
        std::cerr << "There was a problem getting the port number" << std::endl;
    }

    return ntohs(addr.sin_port);
}

bool LinuxSocketServer::isRunning()
{
    return _isRunning;
}