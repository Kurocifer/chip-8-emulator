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
    uint16_t m_pcreg;
    uint16_t m_sp_reg;

    // helper variable
    uint16_t m_current_op; // current opcode being executed.
    uint16_t m_instruction;

    TChip8* m_machine;

    // Logging
    std::shared_ptr<TLogger> m_logger;

public:
    TCpu(TChip8* machine);
    ~TCpu();

    void init();
    void fetch();
    void decode();
    void execute();
    void deinit();
};

#endif