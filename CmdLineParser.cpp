#include <ctype.h>

#include "cmdLineParser.h"

// private functions

void TCmdLineParser::printHelpMessage()
{
    std::cout << "Usage: lcc [options] file..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "   -h, --help              Display this help information" << std::endl;
    std::cout << "   -r, --romFileName       Set the rom file path to be used" << std::endl;
    std::cout << "   -l, --logLevel          Set the desired log level [NONE = 0, ERROR = 1, WARN = 2, INFO = 3, DEBUG = 4]" << std::endl;
}

void TCmdLineParser::setRomFileName(std::string new_name)
{
    this->m_romFileName = new_name;
}

void TCmdLineParser::setLogLevel(int logLevel)
{
    this->m_logLevel = logLevel;
}

// public functions
TCmdLineParser::TCmdLineParser()
    : m_romFileName(""), m_logLevel(-1)
{
    m_logger = TLogger::getInstance();
}

void TCmdLineParser::parseCmdLine(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        std::string auxStr(argv[i]);

        if (auxStr.at(0) == '-')
        {
            // handle command line args
            if (auxStr == "-h" || auxStr == "--help")
            {
                this->printHelpMessage();
                exit(0);
            }

            else if ((auxStr == "-r" || auxStr == "--romFileName") && argc >= 3)
            {
                std::string auxName(argv[++i]);
                this->setRomFileName(auxName);
            }

            else if ((auxStr == "-l" || auxStr == "--logLevel") && argc >= 3)
            {
                // handle setting of rom
                std::string logLevel(argv[++i]);
                if (!std::isdigit(logLevel.at(0)))
                {
                    m_logger->log("Parameter must be a number [0,4]", ELogLevel::ERROR);
                    exit(1);
                }
                this->setLogLevel(stoi(logLevel));
            }

            else
            {
                std::string param(argv[i]);
                m_logger->log("Unknown parameter or not enough arguements: " + param, ELogLevel::ERROR);
                exit(1);
            }
        }
        else
        {
            std::string param(argv[i]);
            m_logger->log("Unknown parameter: " + param, ELogLevel::ERROR);
            exit(1);
        }
    }
}

bool TCmdLineParser::isRomFileNameset()
{
    return !(m_romFileName == "");
}

std::string TCmdLineParser::getRomFileName()
{
    return this->m_romFileName;
}

bool TCmdLineParser::isLogLevelSet()
{
    return !(m_logLevel == -1);
}

int TCmdLineParser::getLogLevel()
{
    return this->m_logLevel;
}