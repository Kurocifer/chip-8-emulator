#include "romLoader.h"

TRomLoader::TRomLoader()
{
    m_logger = TLogger::getInstance();
}

TRomLoader::~TRomLoader()
{

}

void TRomLoader::loadRom(std::string& file_path, uint8_t* mem)
{
    m_logger->log("Loading ROM: " + file_path, ELogLevel::INFO);

    // open the file in binary mode
    std::ifstream file(file_path, std::ios::binary);

    // check if the file was opened successfully
    if (!file)
    {
        m_logger->log("Could not open file: " + file_path, ELogLevel::ERROR);
        exit(1);
    }

    // Determine the rom size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg); // Move back to the begining of the file

    // Read the binray data into the buffer
    if (file.read(reinterpret_cast<char*>(mem), size))
    {
        m_logger->log("File read successfully. Size: " + std::to_string(size) + "bytes. ", ELogLevel::INFO);
    }
    else {
        m_logger->log("Error during file read", ELogLevel::ERROR);
        exit(1);
    }

    file.close();
}