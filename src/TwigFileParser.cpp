#include <TwigFileParser.hpp>

#include <iostream>

TwigFileParser::TwigFileParser(const std::string &filename) : IFileParser(filename), _fileDescriptor(filename) {}
TwigFileParser::~TwigFileParser()
{
    _fileDescriptor.close();
}

void TwigFileParser::setVariables(const std::unordered_map<std::string, std::string> &vars)
{
    _variables = vars;
}

std::string &TwigFileParser::parse()
{
    _parsedValue = "";
    std::string line;
    while (getline(_fileDescriptor, line))
    {
        auto itr = line.find("{");
        while (itr != std::string::npos)
        {
            auto prevFoundPos = line.begin();
            _parsedValue += line.substr(0, itr);
            line = line.substr(itr + 1);
            std::string nextChars = line.substr(0, 2);
            if (nextChars == "{ ")
            {
                /// place variable
                if (auto end = line.find(" }}"))
                {
                    std::string varname = line.substr(2, end - 2);
                    line = line.substr(end + 3);
                    _parsedValue += _variables[varname];
                }
                else
                {
                    _parsedValue += "{";
                }
            } /*else if(nextChars == "% ") {
                ///expression
            } */
            else
            {
                _parsedValue += "{";
            }
            itr = line.find("{");
        }
        _parsedValue += line;
    }
    return _parsedValue;
}