#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>

#include <cstdlib>
#include <signal.h>

#include <curl/curl.h>

#include <HTTPServer.hpp>
#include <Router.hpp>

void test_endpoint(HTTPResponse &res, HTTPRequest &req, Client *c)
{
    int number = std::stoi(req.body);
    res.body = "Your number is " + std::to_string(number + 1);
}

size_t writeResponse(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *response = (std::string *)userdata;
    response->append(ptr, nmemb);
    return nmemb;
}

std::string make_http_call(const std::string &url, const std::string &postData = "")
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "There was an error initialising CURL" << std::endl;
        return "";
    }

    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (postData.size() > 0)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)postData.size());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    res = curl_easy_perform(curl);
    long response_code = 10;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response == "")
    {
        response = std::to_string(response_code);
    }
    curl_easy_cleanup(curl);
    return response;
}

void setup()
{
    Router r;
    r.registerFolderWithStaticFiles("public");
    r.registerRoute("/test", test_endpoint);
}

bool test1(HTTPServer *server)
{
    return make_http_call("http://localhost:" + std::to_string(server->getPort()) + "/index.html") == "Hello, World!\nThis is static!";
}

bool test2(HTTPServer *server)
{
    return make_http_call("http://localhost:" + std::to_string(server->getPort()) + "/test", "3") == "Your number is 4";
}

bool test3(HTTPServer *server)
{
    return make_http_call("http://localhost:" + std::to_string(server->getPort()) + "/does_not_exist") == "404";
}

HTTPServer *server;

int main()
{

    server = new HTTPServer(0);

    while (!server->isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    setup();
    std::cout << "Test 1 " << (test1(server) ? "passed" : "did not pass") << std::endl;
    std::cout << "Test 2 " << (test2(server) ? "passed" : "did not pass") << std::endl;
    std::cout << "Test 3 " << (test3(server) ? "passed" : "did not pass") << std::endl;
    delete server;
    return 0;
}
