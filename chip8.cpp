#include <iostream>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "logger.h"
#include "cpu.h"

TChip8::TChip8()
    : m_emulatorRunning(true) 
{
    m_logger = TLogger::getInstance();
    m_cpu = new TCpu(this);
}

TChip8::~TChip8()
{

}

void TChip8::init(std::string rom_path)
{
    // clear frame buffer
    for (int i = 0; i < SCREEN_HEIGHT; i++)
        for (int j = 0; j < SCREEN_WIDTH; j++)
            m_screen[i][j] = 0;
    
    // clear stack
    for (int i = 0; i < STACK_SIZE; i++)
        m_stack[i] = 0;
    
    // clear RAM
    for (int i = 0; i < TOTAL_RAM; i++)
        m_ram[i] = 0;
    
    // load font set into memory
    for (int i = 0; i < FONTSET_SIZE; i++)
        m_ram[i] = FONTSET[i];
    
    
    // Reset timers
    m_delay_timer = 0;
    m_sound_timer = 0;

    // start keybaord state as all unpressed
    for (int i = 0; i < NUM_KEYS; i++)
        m_Keys[i] = 0;
    
    m_key_pressed = false;

    // Init cpu registers
    m_cpu->init();

    // Load ROM to ram
    m_loader = new TRomLoader();
    m_loader->loadRom(rom_path, m_ram+PROGRAM_START_ADDR);
    delete m_loader;
}

void TChip8::run()
{
    while (m_emulatorRunning)
    {
        m_cpu->fetch();
        m_cpu->decode();
        m_cpu->execute();
    }
    
}

void TChip8::deinit()
{
    m_cpu->deinit();
}