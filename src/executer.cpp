#include "executer.hpp"
#include "cpu.h"         // Aqui incluímos a definição completa da CPU
#include <stdexcept>     // Para lançar exceções
#include <cstdint>

// --- Funções auxiliares internas ---
namespace {

/**
 * @brief Calcula o segundo operando (Operand2).
 * Pode ser um imediato ou um registrador com shift (Barrel Shifter).
 */
uint32_t calcularOperando2(CPU& cpu, const Instrucao& instr) {
    if (instr.operando_shift.eh_valido) {
        uint32_t valor_base = cpu.r[instr.operando_shift.reg_base];
        uint8_t shift = instr.operando_shift.imm_shift;

        switch (instr.operando_shift.tipo) {
            case TipoShift::LSL: return valor_base << shift;
            case TipoShift::LSR: return valor_base >> shift;
            case TipoShift::ASR: return static_cast<int32_t>(valor_base) >> shift;
            case TipoShift::ROR: return (valor_base >> shift) | (valor_base << (32 - shift));
        }
    }
    return instr.valor_imediato;
}

/**
 * @brief Executa a instrução ADD (ou ADDS).
 */
void executar_add(CPU& cpu, const Instrucao& instr) {
    uint32_t op1 = cpu.r[instr.rn];
    uint32_t op2 = calcularOperando2(cpu, instr);
    uint64_t resultado = static_cast<uint64_t>(op1) + static_cast<uint64_t>(op2);

    cpu.r[instr.rd] = static_cast<uint32_t>(resultado);

    if (instr.s_flag) {
        cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
        cpu.setFlagZ(cpu.r[instr.rd] == 0);
        cpu.setFlagC(resultado > 0xFFFFFFFF);
        bool overflow = (~(op1 ^ op2) & (op1 ^ cpu.r[instr.rd])) >> 31;
        cpu.setFlagV(overflow);
    }
}

/**
 * @brief Executa a instrução LDR (carrega da memória).
 */
void executar_ldr(CPU& cpu, const Instrucao& instr) {
    uint32_t base = cpu.r[instr.rn];
    uint32_t offset = calcularOperando2(cpu, instr);
    uint32_t endereco_final = instr.somar_offset ? (base + offset) : (base - offset);

    cpu.r[instr.rd] = cpu.memoria.lerPalavra(endereco_final);

    if (instr.write_back) {
        cpu.r[instr.rn] = endereco_final;
    }
}

/**
 * @brief Executa a instrução STR (armazena na memória).
 */
void executar_str(CPU& cpu, const Instrucao& instr) {
    uint32_t base = cpu.r[instr.rn];
    uint32_t offset = calcularOperando2(cpu, instr);
    uint32_t endereco_final = instr.somar_offset ? (base + offset) : (base - offset);

    cpu.memoria.escreverPalavra(endereco_final, cpu.r[instr.rd]);

    if (instr.write_back) {
        cpu.r[instr.rn] = endereco_final;
    }
}

/**
 * @brief Executa a instrução B (branch).
 */
void executar_b(CPU& cpu, const Instrucao& instr) {
    cpu.r[15] += instr.offset_salto;  // PC já foi incrementado no ciclo da CPU
}

/**
 * @brief Executa a instrução BL (branch with link).
 */
void executar_bl(CPU& cpu, const Instrucao& instr) {
    cpu.r[14] = cpu.r[15];  // Link Register (R14)
    cpu.r[15] += instr.offset_salto;
}

} // fim do namespace interno

// --- Função principal do Executor ---
void executarInstrucao(CPU& cpu, const Instrucao& instr) {
    uint32_t pc_anterior = cpu.r[15];

    switch (instr.opcode) {
        case Opcode::ADD:
            executar_add(cpu, instr);
            break;
        case Opcode::LDR:
            executar_ldr(cpu, instr);
            break;
        case Opcode::STR:
            executar_str(cpu, instr);
            break;
        case Opcode::B:
            executar_b(cpu, instr);
            break;
        case Opcode::BL:
            executar_bl(cpu, instr);
            break;
        case Opcode::INVALIDO:
        default:
            throw std::runtime_error("Erro: Instrução inválida ou não implementada.");
    }

    // Se o PC não foi alterado pela instrução, avança para a próxima
    if (cpu.r[15] == pc_anterior) {
        cpu.r[15] += 4;
    }
}
