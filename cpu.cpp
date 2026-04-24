#include <cstdlib>
#include <ctime>

#include "cpu.h"
#include "logger.h"

TCpu::TCpu(TChip8* machine)
{
    m_machine = machine;
    m_logger = TLogger::getInstance();
}

TCpu::~TCpu()
{

}

void TCpu::init()
{
    // Initialize cpu registers
    m_pcreg = PC_START;
    m_current_op = 0;
    m_sp_reg = 0;
    m_ireg = 0;

    // Clear registers
    for (int i = 0; i < NUM_V_REGISTERS; i++)
        m_reg[i] = 0;
}

void TCpu::fetch()
{
    // Read the opcode spread accross two ram slots (since the ram slot 1 byte and the opcode is 2 bytes)
    // combine them and move the pointer past the two read ram slots
    m_current_op = ((uint16_t)m_machine->m_ram[m_pcreg] << 8) | m_machine->m_ram[m_pcreg+1];
    m_pcreg += 2;
}

void TCpu::decode()
{
    m_instruction = m_current_op >> 12;
}

void TCpu::execute()
{
    m_logger->log("INST " + std::to_string(m_instruction), ELogLevel::DEBUG);

    switch (m_instruction)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF: m_logger->log("INSTRUCTION OPCODE NOT IMPLEMENTED!", ELogLevel::ERROR); exit(1); break;
    
    
    default:
        break;
    }
}

void TCpu::deinit()
{
    
}

