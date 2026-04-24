#ifndef __CPU_H__
#define __CPU_H__

#include <cstdint>

#include "chip8.h"
#include "logger.h"

#define NUM_V_REGISTERS 16
#define PC_START 0X200

class TCpu
{
    // registers
    uint8_t m_reg[NUM_V_REGISTERS];
    uint16_t m_ireg;
    uint16_t m_pcreg;  // Prgram counter
    uint16_t m_sp_reg; // Stack pointer

    // helper variable
    uint16_t m_current_op; // current opcode being executed.
    uint16_t m_instruction;

    TChip8 *m_machine;

    // Logging
    std::shared_ptr<TLogger> m_logger;

    // Instructions

    // Instruction OOEO clear screen
    void clear_screen();

    // Instruction 00EE - Return from a subroutine
    void return_from_subroutine();

    // Instruction 1NNN - Jump to address nnn
    void jump_to();

    // Instruction 2NNN - Call subroutine at nnn
    void call_subroutine();

    // Instruction 3XKK - Skip next instruction if Vx == KK
    void skip_next_instruction_eq();

    // Instruction 4XKK - Skip next instruction if Vx != KK
    void skip_next_instruction_ne();

    // Instruction 5XY0 - Skip next instruction if Vx == Vy
    void skip_next_instruction_vx_vy();

    // Instruction 6XKK - Set register to contant value
    void register_set();

    // Instruction 7XKK - Add constant to register
    void add_reg_imm();

    // Instruction 8XY0 - Load Vx with Vy
    void move_vy_to_vx();

    // Instruction 8XY1 - OR of Vx with Vy
    void or_vx_vy();

    // Instruction 8XY2 - AND of Vx with Vy
    void and_vx_vy();

    // Instruction 8XY3 - XOR of Vx and Vy
    void xor_vx_vy();

    // Instruction 8XY4 - Add Vx with Vy
    void add_vx_vy();

    // Instruction 8XY5 - Substract Vy from Vx
    void sub_vx_vy();

    // Instruction 8XY6 - Vx divided by 2
    void shift_right_reg();

    // Instruction 8XY7 - Subtract Vx from Vy
    void TCpu::subn_vx_vy();

    // Instruction 8XYE - Vx multiplied by 2
    void TCpu::shift_left_reg();

    // Instruction 9XY0 - Skip next instruction if Vx != Vy
    void TCpu::skip_next_instruction_vx_vy_ne();

    // Instruction ANNN - Load I register with constant
    void TCpu::set_index_register();

    // Instruction BNNN - Jump to location NNN + V0
    void TCpu::jump_with_v0();

    // Instruction CXKK - Generate random number
    void TCpu::generate_random_number();

    // Instruction DXYN - Draw Sprite
    void draw_sprite();

    // Instruction EX9E - Skip next instruction if Vx key is pressed
    void decode_E_instruction();

    // Instruction EEX9E - Skip next instruction if Vx key is pressed
    void skip_next_inst_if_key_pressed();

    // Instruction EXA1 - Skip next instruction if Vx key isn't pressed
    void skip_next_inst_if_not_pressed();

    // Instruction FZZZ -
    void decode_F_instruction();

    // Instruction FX07 - Set register with delay timer
    void load_reg_with_delay_timer();

    // Instruction FX0A - Wait for a key press
    void wait_key_press();

    // Instruction FX15 - Set delay timer with register
    void load_delay_timer_with_reg();

    // Instruction FX18 - Set sound timer with register
    void load_sound_timer_with_reg();

    // Instruction FX1E - Add Vx to I register
    void add_ireg_with_reg();

    // FX29 - Set I register with sprite location
    void load_font_from_vx();

    // Instruction FX33 - Store BCD representation of Vx
    void store_binary_code_decimal_representation();

    // Instruction FX55 - Store registers V0 through Vx in memory starting at location I
    void load_memory_from_regs();

    // Instruction FX65 - Read registers V0 through Vx from memory starting at location I
    void load_regs_from_memory();

public:
    TCpu(TChip8 *machine);
    ~TCpu();

    void init();
    void fetch();
    void decode();
    void execute();
    void deinit();
};

#endif