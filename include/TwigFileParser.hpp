#ifndef TWIG_FILE_PARSER
#define TWIG_FILE_PARSER

#include <interface/FileParser.hpp>

#include <unordered_map>
#include <fstream>

class TwigFileParser : public IFileParser
{
public:
    TwigFileParser(const std::string &);
    ~TwigFileParser();

    void setVariables(const std::unordered_map<std::string, std::string> &);

    std::string &parse();

private:
    std::ifstream _fileDescriptor;
    std::unordered_map<std::string, std::string> _variables;
    std::string _parsedValue;
};

#endif