#include "executer.hpp"
#include "cpu.hpp"
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {

/**
 * @brief Retorna o valor de um registrador para ser usado como operando.
 * APLICA A REGRA DO PC: Se o registrador for o PC (R15), retorna seu valor + 4
 * para emular corretamente o pipeline do ARMv7 (PC+8) em nosso simulador
 * (PC+4).
 */
uint32_t getValorOperando(CPU &cpu, uint8_t reg_idx) {
  if (reg_idx == 15) {
    // O r[15] do nosso simulador já está em "endereço_atual + 4".
    // Para simular o "endereço_atual + 8" do hardware real, somamos 4.
    return cpu.r[15] + 4;
  }
  return cpu.r[reg_idx];
}

uint32_t calcularOperando2(CPU &cpu, const Instrucao &instr) {
  if (instr.operando_shift.eh_valido) {
    // CORRIGIDO: Usa a função auxiliar para ler o registrador base do shift
    uint32_t valor_base = getValorOperando(cpu, instr.operando_shift.reg_base);
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

void executar_add(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar para ler o registrador Rn
  uint32_t op1 = getValorOperando(cpu, instr.rn);
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
  // CORRIGIDO: Usa a função auxiliar para ler o registrador Rn
  uint32_t op1 = getValorOperando(cpu, instr.rn);
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
  // CORRIGIDO: Usa a função auxiliar
  cpu.r[instr.rd] =
      getValorOperando(cpu, instr.rn) & calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}

void executar_orr(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar
  cpu.r[instr.rd] =
      getValorOperando(cpu, instr.rn) | calcularOperando2(cpu, instr);
  if (instr.s_flag) {
    cpu.setFlagN((cpu.r[instr.rd] >> 31) & 1);
    cpu.setFlagZ(cpu.r[instr.rd] == 0);
  }
}

void executar_eor(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar
  cpu.r[instr.rd] =
      getValorOperando(cpu, instr.rn) ^ calcularOperando2(cpu, instr);
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

void executar_ldr(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar para ler o registrador base (Rn)
  uint32_t base = getValorOperando(cpu, instr.rn);
  uint32_t offset =
      calcularOperando2(cpu, instr); // Offset pode ser PC-relativo também
  uint32_t endereco_final =
      instr.somar_offset ? (base + offset) : (base - offset);

  uint32_t valor = cpu.memoria.lerPalavra(endereco_final);
  cpu.r[instr.rd] = valor;

  if (instr.write_back) {
    cpu.r[instr.rn] = endereco_final;
  }
}

void executar_str(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar para ler o registrador base (Rn)
  uint32_t base = getValorOperando(cpu, instr.rn);
  uint32_t offset = calcularOperando2(cpu, instr);
  uint32_t endereco_final =
      instr.somar_offset ? (base + offset) : (base - offset);

  cpu.memoria.escreverPalavra(endereco_final, cpu.r[instr.rd]);

  if (instr.write_back) {
    cpu.r[instr.rn] = endereco_final;
  }
}

void executar_cmp(CPU &cpu, const Instrucao &instr) {
  // CORRIGIDO: Usa a função auxiliar
  uint32_t op1 = getValorOperando(cpu, instr.rn);
  uint32_t op2 = calcularOperando2(cpu, instr);
  uint32_t resultado = op1 - op2;

  cpu.setFlagN((resultado >> 31) & 1);
  cpu.setFlagZ(resultado == 0);
  cpu.setFlagC(op1 >= op2);
  bool overflow = ((op1 ^ op2) & (op1 ^ resultado)) >> 31;
  cpu.setFlagV(overflow);
}

void executar_b(CPU &cpu, const Instrucao &instr) {
  // O PC no simulador (r[15]) está em PC_atual+4.
  // O offset do branch é relativo a PC_atual+8.
  // Novo PC = (PC_atual + 8) + offset_salto.
  // Novo PC = (r[15] + 4) + offset_salto.
  // O "+4" já estava no seu código, então a lógica estava correta. Apenas
  // adicionando um comentário.
  cpu.r[15] += instr.offset_salto + 4;
}

void executar_bl(CPU &cpu, const Instrucao &instr) {
  // Salva o endereço da PRÓXIMA instrução, que é o que r[15] contém neste
  // momento.
  cpu.r[14] = cpu.r[15];
  // A lógica do salto é a mesma da instrução B.
  cpu.r[15] += instr.offset_salto + 4;
}

} // namespace

void executarInstrucao(CPU &cpu, const Instrucao &instr) {
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
}
