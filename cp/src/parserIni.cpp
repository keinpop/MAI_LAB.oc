#include "./parserIni.h"

std::string cmd = "Command = ";
std::string dep = "Dep[] = ";
std::string mutx = "Mutex = ";

void ParserIni::returnResultParsing(std::string fileName)
{
    std::ifstream file(fileName);

    if (!(file.is_open())) {    
        std::cerr << "Unable to open file: " << fileName << std::endl;
    }

    // Temporary variables for working with strings
    std::string line;
    bool sectionInit = false;

    // Counters of position 
    size_t countSectionName = 0;
    size_t countSectionCommand = 0;
    size_t countOfDep = 0;

    size_t startPos = 0;
    size_t length = 0;
    while (std::getline(file, line)) {
        // std::cout << line ;
        if (checkIsSquareBrackets(line)) {
            startPos = 1;
            length = line.size() - 2; 

            std::string extractedName = line.substr(startPos, length);
            _parseRersult.push_back({extractedName, {}, {}});

            ++countSectionName;
            countOfDep = 0;
            sectionInit = true;
            continue;
        } else if (checkIsCommandTag(line)) {
            if (!sectionInit) {
                std::perror("Error! Section is not inited");
                exit(EXIT_FAILURE);
            }

            checkingQuotes(line, cmd);

            startPos = cmd.size() + 1;
            length = line.size() - startPos - 1;
            
            std::string extractedCommand = line.substr(startPos, length);
            _parseRersult[countSectionCommand].command = extractedCommand;

            sectionInit = false;
        } else if (checkIsDep(line)) { 
            checkingQuotes(line, dep);

            startPos = dep.size() + 1;
            length = line.size() - startPos - 1;
            
            std::string extractedDep = line.substr(startPos, length);
            _parseRersult[countSectionCommand].dependencies.push_back(extractedDep);

            ++countOfDep;
        } else if (checkIsMutx(line)) {
            checkingQuotes(line, mutx);

            startPos = mutx.size() + 1;
            length = line.size() - startPos - 1;

            if (!(length > 0)) {
                throw std::length_error("Error! Mutex name is undefiend");
            }

            std::string extractedMutx = line.substr(startPos, length);

            _parseRersult[countSectionCommand].mutexName = extractedMutx;
            ++countSectionCommand;
        } else if ((line.size() != 0)) {
            std::perror("Error! Unknow command");
            std::cout << "\n\t" << line << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    file.close();
}

std::vector<Job> ParserIni::getParserRes()
{
    return this->_parseRersult;
}

void ParserIni::checkingQuotes(std::string line, std::string tag)
{
    if (line[tag.size()] != '"' || line[line.size() - 1] != '"' ) {
        std::cout << "Error! Invalid input\n" << "\t" << line << std::endl;
        exit(EXIT_FAILURE);
    }
}

bool ParserIni::checkIsCommandTag(std::string line)
{   
    for (size_t i = 0; i < cmd.size(); ++i) {
        if (line[i] != cmd[i]) {
            return false;
        }
    }

    return true;
}

bool ParserIni::checkIsSquareBrackets(std::string line)
{   
    if (line[0] == '[' || line[line.size() - 1] == ']') {
        return true;
    }

    return false;
}

bool ParserIni::checkIsDep(std::string line)
{
    for (size_t i = 0; i < dep.size(); ++i) {
        if (line[i] != dep[i]) {
            return false;
        }
    }

    return true;
}

bool ParserIni::checkIsMutx(std::string line)
{
    for (size_t i = 0; i < dep.size(); ++i) {
        if (line[i] != mutx[i]) {
            return false;
        }
    }

    return true;
}