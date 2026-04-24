#include <cstdlib>
#include <ctime>

#include "cpu.h"
#include "logger.h"

TCpu::TCpu(TChip8 *machine)
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
    m_current_op = ((uint16_t)m_machine->m_ram[m_pcreg] << 8) | m_machine->m_ram[m_pcreg + 1];
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
    case 0xF:
        m_logger->log("INSTRUCTION OPCODE NOT IMPLEMENTED!", ELogLevel::ERROR);
        exit(1);
        break;

    default:
        break;
    }
}

void TCpu::deinit()
{
}

void TCpu::clear_screen()
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            m_machine->m_screen[i][j] = 0;
        }
    }
}

void TCpu::return_from_subroutine()
{
    m_sp_reg--;
    m_pcreg = m_machine->m_stack[m_sp_reg];
}

void TCpu::jump_to()
{
    m_pcreg = m_current_op & 0x0FFF;
}

void TCpu::call_subroutine()
{
    uint16_t nnn = m_current_op & 0x0FFF;

    m_machine->m_stack[m_sp_reg] = m_pcreg;
    m_sp_reg++;
    m_pcreg = nnn;
}

void TCpu::skip_next_instruction_eq()
{
    uint8_t value = m_current_op & 0XFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    if (m_reg[reg] == value)
        m_pcreg += 2;
}

void TCpu::skip_next_instruction_ne()
{
    uint8_t value = m_current_op & 0XFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    if (m_reg[reg] != value)
        m_pcreg += 2;
}

void TCpu::skip_next_instruction_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if (m_reg[reg_x] == m_reg[reg_y])
        m_pcreg += 2;
}

void TCpu::register_set()
{
    uint8_t value = m_current_op & 0xFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    m_reg[reg] = value;
}

void TCpu::add_reg_imm()
{
    uint8_t value = m_current_op & 0xFF;
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    m_reg[reg] += value;
}

void TCpu::move_vy_to_vx()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    m_reg[reg_x] = m_reg[reg_y];
}

void TCpu::or_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    m_reg[reg_x] |= m_reg[reg_y];
}

void TCpu::xor_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    m_reg[reg_x] ^= m_reg[reg_y];
}

void TCpu::add_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;
    uint16_t add = m_reg[reg_x] + m_reg[reg_y];

    if (add > 0xFF)
        m_reg[0xF] = 1;
    else
        m_reg[0xF] = 0;

    m_reg[reg_x] = add & 0xFF;
}

void TCpu::sub_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if (m_reg[reg_x] > m_reg[reg_y])
        m_reg[0xF] = 1;
    else
        m_reg[0xF] = 0;

    m_reg[reg_x] = m_reg[reg_y];
}

void TCpu::shift_right_reg()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    if (m_reg[reg] % 2 == 1)
        m_reg[0xF] = 1;
    else
        m_reg[0xF] = 0;

    m_reg[reg] >>= 1;
}

void TCpu::subn_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if(m_reg[reg_y] > m_reg[reg_x])
        m_reg[0xF] = 1;
    else
        m_reg[0xF] = 0;

    m_reg[reg_x] = m_reg[reg_y] - m_reg[reg_x];
}

void TCpu::shift_left_reg()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    if(m_reg[reg] & 0x80 == 1 )
        m_reg[0xF] = 1;
    else
        m_reg[0xF] = 0;

    m_reg[reg] <<= 1;
}

void TCpu::skip_next_instruction_vx_vy_ne()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    if(m_reg[reg_x] != m_reg[reg_y])
        m_pcreg += 2;
}

void TCpu::set_index_register()
{
    m_ireg = m_current_op & 0x0FFF;
}

void TCpu::jump_with_v0()
{
    uint16_t nnn = m_current_op & 0x0FFF;

    m_pcreg = nnn + m_reg[0];
}

void TCpu::generate_random_number()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    uint8_t kk = m_current_op & 0xFF;
    uint8_t randNum = rand() % 256;

    m_reg[reg] = randNum & kk;
}