#include <iostream>
#include "logger.h"
#include "cmdLineParser.h"
#include "chip8.h"
#include "displaysdl.h"
#include "keyboardsdl.h"
#include "soundsdl.h"

int main(int argc, char **argv)
{
    TCmdLineParser cmdParser;
    std::shared_ptr<TLogger> logger = TLogger::getInstance();

    cmdParser.parseCmdLine(argc, argv);

    if (cmdParser.isLogLevelSet())
    {
        switch (cmdParser.getLogLevel())
        {
            case 0: logger->setLogLevel(ELogLevel::NONE); break;
            case 1: logger->setLogLevel(ELogLevel::ERROR); break;
            case 2: logger->setLogLevel(ELogLevel::WARN); break;
            case 3: logger->setLogLevel(ELogLevel::INFO); break;
            case 4: logger->setLogLevel(ELogLevel::DEBUG); break;
        }
    }

    if(!cmdParser.isRomFileNameset())
    {
        logger->log("No ROM path provided", ELogLevel::ERROR);
        exit(1);
    }

    logger->log("ROM Path: " +cmdParser.getRomFileName(), ELogLevel::INFO);

    TDisplaySDL display;
    TKeyboardSDL keyboard;
    TSoundSDL sound;
    TChip8 emulator;
    emulator.setDisplay(&display);
    emulator.setKeyboard(&keyboard);
    emulator.setSound(&sound);
    emulator.init(cmdParser.getRomFileName());
    emulator.run();
    emulator.deinit();

    return 0;
}