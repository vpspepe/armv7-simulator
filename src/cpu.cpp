#include "cpu.hpp"
#include "decoder.hpp"
#include "executer.hpp"
#include <iostream>
#include <iomanip>

// No construtor, inicializamos o estado da CPU.
CPU::CPU(size_t memoria_bytes) : memoria(memoria_bytes), cpsr(0) {
    // Zera todos os registradores
    r.fill(0);
}

void CPU::carregarPrograma(const std::string& nome_arquivo, uint32_t endereco_inicial) {
    memoria.carregarDeArquivo(nome_arquivo, endereco_inicial);
    r[15] = endereco_inicial; // Define o Program Counter para o início do programa
}

// O loop principal de execução do simulador.
void CPU::executar() {
    while (true) {
        // FETCH
        // Busca a instrução de 32 bits do endereço apontado pelo PC (R15)
        uint32_t instrucao_binaria = memoria.lerPalavra(r[15]);
        
        // Em um processador real, o PC é incrementado durante o ciclo.
        // Faremos isso antes da execução para simplificar a lógica de saltos.
        uint32_t pc_atual = r[15];
        r[15] += 4;

        // Condição de parada
        if (instrucao_binaria == 0) {
            std::cout << "--- Fim da Simulação (instrução nula encontrada) ---\n";
            imprimirEstado();
            break;
        }

        // DECODE
        // Envia a instrução binária para o módulo decodificador.
        Instrucao instr = decodificar(instrucao_binaria);

        // EXECUTE
        // Primeiro, verifica se a condição da instrução é satisfeita.
        if (checarCondicao(instr.cond)) {
            // Se sim, envia a instrução decodificada e uma referência de si mesma
            // para o módulo executor realizar a operação.
            executarInstrucao(*this, instr);
        }
        // Se a condição não for satisfeita, a instrução é ignorada e o PC
        // simplesmente continua com o valor já incrementado.
    }
}

// Imprime o estado atual dos registradores e flags
void CPU::imprimirEstado() {
    std::cout << "--------------------------------\n";
    std::cout << "Estado dos Registradores:\n";
    std::cout << "--------------------------------\n";
    for (int i = 0; i < 16; ++i) {
        // Alinha o nome do registrador à esquerda com 8 caracteres de espaço
        std::cout << std::left << std::setw(8);

        // Registradores de propósito especial
        if (i < 13) {
            std::cout << ("R" + std::to_string(i));
        } else if (i == 13) {
            std::cout << "SP (R13)";
        } else if (i == 14) {
            std::cout << "LR (R14)";
        } else {
            std::cout << "PC (R15)";
        }

        // Imprime o valor em hexadecimal e em decimal
        std::cout << ": 0x" << std::hex << std::setfill('0') << std::setw(8) << r[i]
                  << "   (" << std::dec << r[i] << ")\n";
    }

    // Imprime o CPSR
    std::cout << "\nCPSR: 0x" << std::hex << std::setfill('0') << std::setw(8) << cpsr << std::dec << "\n";

    // Imprime as Flags
    std::cout << "Flags: [ N=" << getFlagN() << " Z=" << getFlagZ() 
              << " C=" << getFlagC() << " V=" << getFlagV() << " ]\n";
    std::cout << "--------------------------------\n";
}

// Implementação da verificação de condição
bool CPU::checarCondicao(Condicao cond) {
    bool N = getFlagN();
    bool Z = getFlagZ();
    bool C = getFlagC();
    bool V = getFlagV();

    switch (cond) {
        case Condicao::EQ: return Z;
        case Condicao::NE: return !Z;
        case Condicao::CS: return C;
        case Condicao::CC: return !C;
        case Condicao::MI: return N;
        case Condicao::PL: return !N;
        case Condicao::VS: return V;
        case Condicao::VC: return !V;
        case Condicao::HI: return C && !Z;
        case Condicao::LS: return !C || Z;
        case Condicao::GE: return N == V;
        case Condicao::LT: return N != V;
        case Condicao::GT: return !Z && (N == V);
        case Condicao::LE: return Z || (N != V);
        case Condicao::AL: return true;
        default: return false;
    }
}

// Helpers de manipulação de flags
// Flag N (Negative)
bool CPU::getFlagN() { return (cpsr >> 31) & 1; }
void CPU::setFlagN(bool valor) {
    if (valor) {
        cpsr |= (1U << 31);
    } else {
        cpsr &= ~(1U << 31);
    }
}

// Flag Z (Zero)
bool CPU::getFlagZ() { return (cpsr >> 30) & 1; }
void CPU::setFlagZ(bool valor) {
    if (valor) {
        cpsr |= (1U << 30);
    } else {
        cpsr &= ~(1U << 30);
    }
}

// Flag C (Carry)
bool CPU::getFlagC() { return (cpsr >> 29) & 1; }
void CPU::setFlagC(bool valor) {
    if (valor) {
        cpsr |= (1U << 29);
    } else {
        cpsr &= ~(1U << 29);
    }
}

// Flag V (Overflow)
bool CPU::getFlagV() { return (cpsr >> 28) & 1; }
void CPU::setFlagV(bool valor) {
    if (valor) {
        cpsr |= (1U << 28);
    } else {
        cpsr &= ~(1U << 28);
    }
}
