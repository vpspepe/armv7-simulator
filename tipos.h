#pragma once
#include <cstdint> // Para usar tipos de tamanho fixo como uint32_t e uint8_t

/**
 * @brief Enumerações para tornar o código mais legível e seguro.
 * Usamos 'enum class' para evitar que os valores poluam o escopo global.
 */

// Define todos os tipos de instrução que nosso simulador irá suportar.
enum class Opcode {
    ADD, SUB, AND, ORR, EOR, MOV, CMP, // ULA
    LDR, STR,                          // Memória
    B, BL,                             // Controle de Fluxo
    INVALIDO                          // Para instruções não reconhecidas
};

// Define todas as 14 condições de execução do ARM (+ a incondicional 'AL').
enum class Condicao {
    EQ, NE, CS, CC, MI, PL, VS, VC,
    HI, LS, GE, LT, GT, LE, AL
};

// Define os tipos de operação do Barrel Shifter.
enum class TipoShift {
    LSL, LSR, ASR, ROR
};

// Define os modos de endereçamento para LDR/STR.
enum class ModoIndexacao {
    PreIndexado,
    PosIndexado
};


/**
 * @brief Estruturas de dados para passar informações entre os módulos.
 */

// Estrutura para encapsular os detalhes de um operando do tipo registrador com shift.
struct OperandoShift {
    bool eh_valido = false;     // Este operando foi usado na instrução?
    TipoShift tipo;             // Qual shift? LSL, LSR...
    uint8_t reg_base;           // O registrador a ser deslocado (Rm)
    uint8_t imm_shift;          // A quantidade imediata de shift
};

// A estrutura principal que representa uma instrução completamente decodificada.
// O Decodificador preenche esta struct, e o Executor a utiliza para trabalhar.
struct Instrucao {
    Opcode opcode = Opcode::INVALIDO;
    Condicao cond = Condicao::AL;
    bool s_flag = false;       // O 'S' das instruções de ULA (ex: ADDS) foi setado?
    uint8_t rn = 0, rd = 0;    // Registradores principais (primeiro operando e destino)

    // --- Campos para instruções de DADOS (ULA) ---
    uint32_t valor_imediato;      // Valor da constante, se aplicável
    OperandoShift operando_shift; // Detalhes do Barrel Shifter, se aplicável

    // --- Campos para instruções de MEMÓRIA ---
    bool write_back = false;      // Bit 'W': Atualizar o registrador base?
    ModoIndexacao modo_idx;       // Bit 'P': Pré ou Pós-indexação?
    bool somar_offset = true;   // Bit 'U': Somar ou subtrair o offset?

    // --- Campos para instruções de SALTO ---
    int32_t offset_salto = 0;   // Deslocamento para a instrução B ou BL
};
