#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

typedef struct _job {
    std::string name;
    std::string command;
    std::vector<std::string> dependencies;
    std::string mutexName;
} Job;

class ParserIni 
{
public:
    ParserIni() = default;

    ~ParserIni() = default;

    void returnResultParsing(std::string fileName);
    std::vector<Job> getParserRes();

private:
    void checkingQuotes(std::string line, std::string tag);
    bool checkIsCommandTag(std::string line);
    bool checkIsSquareBrackets(std::string line);
    bool checkIsDep(std::string line);
    bool checkIsMutx(std::string line);

private:
    std::vector<Job> _parseRersult;
};