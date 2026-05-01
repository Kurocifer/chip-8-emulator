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
    case 0x0:   decode_0_instruction(); break;
    case 0x1:   jump_to(); break;  
    case 0x2:   call_subroutine(); break;
    case 0x3:   skip_next_instruction_eq(); break;
    case 0x4:   skip_next_instruction_ne(); break;
    case 0x5:   skip_next_instruction_vx_vy(); break;
    case 0x6:   register_set(); break;
    case 0x7:   add_reg_imm(); break;
    case 0x8:   decode_8_instruction(); break;
    case 0x9:   skip_next_instruction_vx_vy_ne(); break;
    case 0xA:   set_index_register(); break;
    case 0xB:   jump_with_v0(); break;
    case 0xC:   generate_random_number(); break;
    case 0xD:   draw_sprite(); break;
    case 0xE:   decode_E_instruction(); break;
    case 0xF:   decode_F_instruction(); break;

    default:
        m_logger->log("Unknown Instruction" + std::to_string(m_instruction), ELogLevel::ERROR);
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

void TCpu::draw_sprite()
{
    uint8_t v_reg_x = (m_current_op & 0x0F00) >> 8;
    uint8_t v_reg_y = (m_current_op & 0x00F0) >> 4;   
    uint8_t sprite_height = m_current_op & 0x000F;
    uint8_t x_location = m_reg[v_reg_x];
    uint8_t y_location = m_reg[v_reg_y];

    // Reset colision register
    m_reg[0xF] = 0;
    for(int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++)
    {
        uint8_t pixel = m_machine->m_ram[m_ireg + y_coordinate];
        for(int x_coordinate = 0; x_coordinate < 8; x_coordinate++)
        {
            if((pixel & (0x80 >> x_coordinate)) != 0)
            { 
                if(m_machine->m_screen[y_location + y_coordinate][x_location + x_coordinate] == 1)
                    m_reg[0xF] = 1;
                
                m_machine->m_screen[y_location + y_coordinate][x_location + x_coordinate] ^= 0x1;
            }
        }
    }
}

void TCpu::decode_E_instruction()
{
    switch(m_current_op & 0xFF)
    {
        case 0x009E: skip_next_inst_if_key_pressed(); break;
        case 0x00A1: skip_next_inst_if_not_pressed(); break;
        default: m_logger->log("Instruction E with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
}

void TCpu::skip_next_inst_if_key_pressed()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    uint8_t val = m_reg[reg];

    if(m_machine->m_keys[val] != 0)
        m_pcreg += 2;
}

void TCpu::skip_next_inst_if_not_pressed()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    uint8_t val = m_reg[reg];

    if(m_machine->m_keys[val] == 0)
        m_pcreg += 2;
}

void TCpu::decode_F_instruction()
{
    switch(m_current_op & 0xFF)
    {
        case 0x0007: load_reg_with_delay_timer(); break;
        case 0x000A: wait_key_press(); break;
        case 0x0015: load_delay_timer_with_reg(); break;
        case 0x0018: load_sound_timer_with_reg(); break;
        case 0x001E: add_ireg_with_reg(); break;
        case 0x0029: load_font_from_vx(); break;
        case 0x0033: store_binary_code_decimal_representation(); break;
        case 0x0055: load_memory_from_regs(); break;
        case 0x0065: load_regs_from_memory(); break;
        default: m_logger->log("Instruction F with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
}

void TCpu::load_reg_with_delay_timer()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_reg[reg] = m_machine->m_delay_timer;
}

void TCpu::wait_key_press()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    bool key_pressed = false;

    for(int i=0; i<NUM_KEYS; i++)
    {
        if(m_machine->m_keys[i] != 0)
        {
            m_reg[reg] = i;
            key_pressed = true;
        }
    }

    if(!key_pressed)
        m_pcreg -= 2;
}

void TCpu::load_delay_timer_with_reg()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_machine->m_delay_timer = m_reg[reg];
}

void TCpu::load_sound_timer_with_reg()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_machine->m_sound_timer = m_reg[reg];
}

void TCpu::add_ireg_with_reg()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_ireg += m_reg[reg];
}

void TCpu::load_font_from_vx()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;
    m_ireg = m_reg[reg] * 0x5;
}

void TCpu::store_binary_code_decimal_representation()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    m_machine->m_ram[m_ireg] = m_reg[reg] / 100;
    m_machine->m_ram[m_ireg+1] = (m_reg[reg] / 10) % 10;
    m_machine->m_ram[m_ireg+1] = (m_reg[reg] % 100) % 10;
}

void TCpu::load_memory_from_regs()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    for(int i=0; i<=reg; i++)
        m_machine->m_ram[m_ireg + i] = m_reg[i];

    m_ireg += (reg + 1);
}

void TCpu::load_regs_from_memory()
{
    uint8_t reg = (m_current_op >> 8) & 0x0F;

    for(int i=0; i<=reg; i++)
        m_reg[i] = m_machine->m_ram[m_ireg + i];

    m_ireg += (reg + 1);
}

void TCpu::decode_0_instruction()
{
    switch(m_current_op & 0xFF)
    {
        case 0xE0: clear_screen(); break;
        case 0xEE: return_from_subroutine(); break;
        default: m_logger->log("Instruction 0 with code " + std::to_string(m_current_op & 0xFF), ELogLevel::ERROR);
    }
}

void TCpu::decode_8_instruction()
{
    switch(m_current_op & 0xF)
    {
        case 0x0: move_vy_to_vx(); break;
        case 0x1: or_vx_vy(); break;
        case 0x2: and_vx_vy(); break;
        case 0x3: xor_vx_vy(); break;
        case 0x4: add_vx_vy(); break;
        case 0x5: sub_vx_vy(); break;
        case 0x6: shift_right_reg(); break;
        case 0x7: subn_vx_vy(); break;
        case 0xE: shift_left_reg(); break;
        default: m_logger->log("Instruction 8 with code " + std::to_string(m_current_op & 0xF), ELogLevel::ERROR);
    }
}

void TCpu::and_vx_vy()
{
    uint8_t reg_x = (m_current_op >> 8) & 0x0F;
    uint8_t reg_y = (m_current_op >> 4) & 0x0F;

    m_reg[reg_x] &= m_reg[reg_y];
}