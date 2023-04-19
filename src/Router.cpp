#include <Router.hpp>
#include <HTTPStatusPhrases.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unistd.h>

namespace fs = std::filesystem;

bool is_subpath(const fs::path &path, const fs::path &base)
{
    auto rel = fs::relative(path, base);
    return !rel.empty() && rel.native()[0] != '.';
}

std::string read_file(std::string_view path)
{
    constexpr size_t read_size = std::size_t(4096);
    std::ifstream stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    std::string out;
    std::string buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size))
    {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

std::unordered_map<std::string, requestHandler> Router::_paths = std::unordered_map<std::string, requestHandler>();
std::vector<std::string> Router::_static_files_folder = std::vector<std::string>();

Router::Router()
{
}
Router::~Router() {}

int Router::serveStatic(HTTPResponse &res, HTTPRequest &req, Client *who)
{
    for (std::string &path : _static_files_folder)
    {
        fs::path abs_path = fs::current_path() / path / req.fileRequested.substr(1);
        if (is_subpath(abs_path, fs::current_path() / path) && fs::exists(abs_path))
        {
            res.httpVersion = 1.1;
            res.body = read_file(abs_path.generic_string());
            res.statusCode = 200;
            res.stringResponse = statusCodeToPhrase.at(200);
            return 0;
        }
    }
    return -1;
}

void Router::registerFolderWithStaticFiles(const std::string &path)
{
    if (std::find(_static_files_folder.begin(), _static_files_folder.end(), path) == _static_files_folder.end())
    {
        _static_files_folder.push_back(path);
    }
    else
    {
        std::cerr << "Error: Folder already registered!\n";
        _exit(-1);
    }
}

void Router::registerRoute(const std::string &path, requestHandler callback)
{
    if (_paths.find(path) == _paths.end())
    {
        _paths[path] = callback;
    }
    else
    {
        std::cerr << "Error: Path already registered!\n";
        _exit(-1);
    }
}

int Router::request(HTTPResponse &res, HTTPRequest &req, Client *who)
{
    if (serveStatic(res, req, who) == 0)
    {
        return 0;
    }
    if (_paths.find(req.fileRequested) != _paths.end())
    {
        _paths[req.fileRequested](res, req, who);
        return 0;
    }

    std::cerr << "Error: 404, Path " + req.fileRequested + " not found!\n";
    return -1;
}

int Router::redirectToRoute(const std::string &path, HTTPResponse &response)
{
    if (_paths.find(path) != _paths.end())
    {
        response = createRedirectResponse(path);
        return 0;
    }
    else
    {
        std::cerr << "Error: Redirect route not found!\n";
        return -1;
    }
}

HTTPResponse Router::createRedirectResponse(const std::string &path)
{
    HTTPResponse response;
    response.httpVersion = 3;
    response.statusCode = 307;
    response.fields["Location"] = path;
    return response;
}