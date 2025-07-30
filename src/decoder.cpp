#include "decoder.hpp"
#include <stdexcept>

namespace {
    Instrucao decodificarProcessamentoDeDados(uint32_t instr_binaria) {
        Instrucao instr;

        // Bits de condição
        instr.cond = static_cast<Condicao>((instr_binaria >> 28) & 0b1111);

        // Flag S
        instr.s_flag = (instr_binaria >> 20) & 1;

        // Registradores Rn e Rd
        instr.rn = (instr_binaria >> 16) & 0b1111;
        instr.rd = (instr_binaria >> 12) & 0b1111;

        // Opcode
        uint8_t opcode_val = (instr_binaria >> 21) & 0b1111;
        switch(opcode_val) {
            case 0b0100: instr.opcode = Opcode::ADD; break;
            case 0b0010: instr.opcode = Opcode::SUB; break;
            case 0b0000: instr.opcode = Opcode::AND; break;
            case 0b1100: instr.opcode = Opcode::ORR; break;
            case 0b0001: instr.opcode = Opcode::EOR; break;
            case 0b1101: instr.opcode = Opcode::MOV; break;
            case 0b1010: instr.opcode = Opcode::CMP; break;
            default: instr.opcode = Opcode::INVALIDO; break;
        }

        // Extração do segundo operando (imediato ou registrador)
        bool eh_imediato = (instr_binaria >> 25) & 1;
        if (eh_imediato) {
            uint8_t imediato_8bits = instr_binaria & 0xFF;
            uint8_t fator_rotacao = (instr_binaria >> 8) & 0b1111;
            uint32_t rot = fator_rotacao * 2;
            instr.valor_imediato = (imediato_8bits >> rot) | (imediato_8bits << (32 - rot));
        } else {
            instr.operando_shift.eh_valido = true;
            instr.operando_shift.reg_base = instr_binaria & 0b1111;
            instr.operando_shift.tipo = static_cast<TipoShift>((instr_binaria >> 5) & 0b11);
            instr.operando_shift.imm_shift = (instr_binaria >> 7) & 0b11111;
        }
        return instr;
    }
    Instrucao decodificarAcessoMemoria(uint32_t instr_binaria) {
        Instrucao instr;

        // Bits de condição
        instr.cond = static_cast<Condicao>((instr_binaria >> 28) & 0b1111);

        // Flag S
        instr.s_flag = (instr_binaria >> 20) & 1;

        // Registradores Rn e Rd
        instr.rn = (instr_binaria >> 16) & 0b1111;
        instr.rd = (instr_binaria >> 12) & 0b1111;

        // Verifica bit L
        bool eh_load = (instr_binaria >> 20) & 1;
        if (eh_load) {
            instr.opcode = Opcode::LDR;
        }
        else {
            instr.opcode = Opcode::STR;
        }

        // Modo de endereçamento
        // Bit P (pré/pós-indexado)
        bool eh_pre_idx = (instr_binaria >> 24) & 1;
        instr.modo_idx = eh_pre_idx ? ModoIndexacao::PreIndexado : ModoIndexacao::PosIndexado;

        // Bit U (somar/subtrair da base)
        instr.somar_offset = (instr_binaria >> 23) & 1;

        // Bit W (alterar registrado base)
        instr.write_back = (instr_binaria >> 21) & 1;

        // Offset com registrador de índice?
        bool eh_offset_registrador = (instr_binaria >> 25) & 1;
    
        if (eh_offset_registrador) {
            instr.operando_shift.eh_valido = true;
            instr.operando_shift.reg_base = instr_binaria & 0b1111;
            instr.operando_shift.tipo = static_cast<TipoShift>((instr_binaria >> 5) & 0b11);
            instr.operando_shift.imm_shift = (instr_binaria >> 7) & 0b11111;
        }
        // Offset com imediato de índice
        else {
            instr.valor_imediato = instr_binaria & 0xFFF;
        }
        return instr;
    }
    Instrucao decodificarSalto(uint32_t instr_binaria) {
        Instrucao instr;

        // Bits de condição
        instr.cond = static_cast<Condicao>((instr_binaria >> 28) & 0b1111);

        // Branch ou Branch and Link?
        bool eh_branch_link = (instr_binaria >> 24) & 1;
        if (eh_branch_link) {
            instr.opcode = Opcode::BL;
        }
        else {
            instr.opcode = Opcode::B;
        }

        // Offset
        int32_t offset = instr_binaria & 0x00FFFFFF;

        // Extensão de sinal caso o número seja negativo
        if (offset & 0x00800000) {
            offset |= 0xFF000000;
        }

        // Multiplicação por 4, pois o offset na instrução é múltiplo de 4
        offset <<= 2;

        instr.offset_salto = offset;
        
        return instr;
    } 
}

Instrucao decode(uint32_t instr_binaria) {
    uint8_t type = (instr_binaria >> 25) & 0b111;

    // Tipo da instrução
    if (type == 0b000 || type == 0b001) {
        return decodificarProcessamentoDeDados(instr_binaria);
    }
    else if (type == 0b010 || type == 0b011) {
        return decodificarAcessoMemoria(instr_binaria);
    }
    else if (type == 0b101) {
        return decodificarSalto(instr_binaria);
    }

    // Se não for de nenhum dos tipos, retorna uma instrução inválida
    Instrucao invalid_instr;
    invalid_instr.opcode = Opcode::INVALIDO;
    return invalid_instr;
    
}