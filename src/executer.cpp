#include "executer.hpp"
#include "cpu.hpp" // Aqui incluímos a definição completa da CPU
#include <bitset>
#include <cstdint>
// #include <format>    // Para std::string e formatação de strings
#include <stdexcept> // Para lançar exceções

#include <fstream> // Para std::ifstream (leitura de arquivos)
#include <iomanip>
#include <iostream>  // Para std::cerr (saída de erro)
#include <stdexcept> // Para std::runtime_error e std::out_of_range
// namespace interno para evitar conflitos de nomes
// estas funções não podem ser acessadas de outros arquivos dentro do projeto
namespace {

/**
 * @brief Calcula o segundo operando (Operand2).
 * Pode ser um imediato ou um registrador com shift (Barrel Shifter).
 */
uint32_t calcularOperando2(CPU &cpu, const Instrucao &instr) {
  if (instr.operando_shift.eh_valido) {
    uint32_t valor_base = cpu.r[instr.operando_shift.reg_base];
    uint8_t shift = instr.operando_shift.imm_shift;

    switch (instr.operando_shift.tipo) {
    case TipoShift::LSL:
      return valor_base << shift;
    case TipoShift::LSR:
      return valor_base >> shift;
    case TipoShift::ASR:
      return static_cast<int32_t>(valor_base) >> shift;
    case TipoShift::ROR:
      return (valor_base >> shift) | (valor_base << (32 - shift));
    }
  }
  return instr.valor_imediato;
}

/**
 * @brief Executa a instrução ADD (ou ADDS).
 */
void executar_add(CPU &cpu, const Instrucao &instr) {
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

void executar_sub(CPU &cpu, const Instrucao &instr) {
  uint32_t op1 = cpu.r[instr.rn];
  uint32_t op2 = calcularOperando2(cpu, instr);
  uint64_t resultado = static_cast<uint64_t>(op1) - op2;
  cpu.r[instr.rd] = static_cast<uint32_t>(resultado);

  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
    cpu.setFlagC(op1 >= op2);
    bool overflow = ((op1 ^ op2) & (op1 ^ cpu.r[instr.rd])) >> 31;
    cpu.setFlagV(overflow);
  }
}

void executar_and(CPU &cpu, const Instrucao &instr) {
  cpu.r[instr.rd] = cpu.r[instr.rn] & calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}
void executar_orr(CPU &cpu, const Instrucao &instr) {
  cpu.r[instr.rd] = cpu.r[instr.rn] | calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}
void executar_eor(CPU &cpu, const Instrucao &instr) {
  cpu.r[instr.rd] = cpu.r[instr.rn] ^ calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}

void executar_mov(CPU &cpu, const Instrucao &instr) {
  cpu.r[instr.rd] = calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}

/**
 * @brief Executa a instrução LDR (carrega da memória).
 */
void executar_ldr(CPU &cpu, const Instrucao &instr) {
  uint32_t base = cpu.r[instr.rn];
  uint32_t offset = calcularOperando2(cpu, instr);
  uint32_t endereco_final =
      instr.somar_offset ? (base + offset) : (base - offset);

  cpu.r[instr.rd] = cpu.memoria.lerPalavra(endereco_final);

  if (instr.write_back) {
    cpu.r[instr.rn] = endereco_final;
  }
}

/**
 * @brief Executa a instrução STR (armazena na memória).
 */
void executar_str(CPU &cpu, const Instrucao &instr) {
  uint32_t base = cpu.r[instr.rn];
  uint32_t offset = calcularOperando2(cpu, instr);
  uint32_t endereco_final =
      instr.somar_offset ? (base + offset) : (base - offset);

  cpu.memoria.escreverPalavra(endereco_final, cpu.r[instr.rd]);

  if (instr.write_back) {
    cpu.r[instr.rn] = endereco_final;
  }
}
void executar_cmp(CPU &cpu, const Instrucao &instr) {
  uint32_t op1 = cpu.r[instr.rn];
  uint32_t op2 = calcularOperando2(cpu, instr);
  uint32_t resultado = op1 - op2;

  cpu.setFlagN((resultado >> 31) & 1);
  cpu.setFlagZ(resultado == 0);
  cpu.setFlagC(op1 >= op2);
  bool overflow = ((op1 ^ op2) & (op1 ^ resultado)) >> 31;
  cpu.setFlagV(overflow);
}

/**
 * @brief Executa a instrução B (branch).
 */
void executar_b(CPU &cpu, const Instrucao &instr) {
  cpu.r[15] += instr.offset_salto; // PC já foi incrementado no ciclo da CPU
}

/**
 * @brief Executa a instrução BL (branch with link).
 */
void executar_bl(CPU &cpu, const Instrucao &instr) {
  cpu.r[14] = cpu.r[15]; // Link Register (R14)
  cpu.r[15] += instr.offset_salto;
}

} // namespace

// Esta é a função que é chamada no cpu.cpp
void executarInstrucao(CPU &cpu, const Instrucao &instr) {
  uint32_t pc_anterior = cpu.r[15];

  switch (instr.opcode) {
  case Opcode::ADD:
    executar_add(cpu, instr);
    break;
  case Opcode::SUB:
    executar_sub(cpu, instr);
    break;
  case Opcode::AND:
    executar_and(cpu, instr);
    break;
  case Opcode::ORR:
    executar_orr(cpu, instr);
    break;
  case Opcode::EOR:
    executar_eor(cpu, instr);
    break;
  case Opcode::MOV:
    executar_mov(cpu, instr);
    break;
  case Opcode::CMP:
    executar_cmp(cpu, instr);
    break;
  // Instruções de memória
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
    std::cout << "INSTRUÇÃO INVÁLIDA: 0b" << std::bitset<4>(int((instr.opcode)))
              << "\nPC : 0x" << std::hex << pc_anterior << std::dec << '\n';
    throw std::runtime_error("Erro: Instrução inválida ou não implementada.");
  }
}
