#ifndef FILE_PARSER_HPP
#define FILE_PARSER_HPP

#include <string>
#include <iostream>

static std::string s;

class IFileParser
{
public:
    IFileParser(std::string fileName) {}
    virtual ~IFileParser() {}

    virtual std::string &parse()
    {
        std::cerr << "File parser not implemented" << std::endl;
        return s;
    }
};

#endif