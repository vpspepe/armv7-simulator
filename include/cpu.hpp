#pragma once
#include <array>
#include <string>
#include "memoria.hpp"
#include "tipos.h"

struct Instrucao;
Instrucao decodificar(uint32_t instrucao_binaria);
void executarInstrucao(class CPU& cpu, const Instrucao& instr);


/**
 * @class CPU
 * @brief Simula o estado e o ciclo de execução de um processador ARM.
 */
class CPU {
public:
    /**
     * @brief Construtor da CPU.
     * @param memoria_bytes O tamanho da memória RAM a ser alocada para o simulador.
     */
    CPU(size_t memoria_bytes);

    /**
     * @brief Carrega um programa binário na memória da CPU.
     * @param nome_arquivo O caminho para o arquivo binário.
     * @param endereco_inicial Endereço onde o programa será carregado.
     */
    void carregarPrograma(const std::string& nome_arquivo, uint32_t endereco_inicial);

    /**
     * @brief Inicia o loop principal de simulação (Fetch-Decode-Execute).
     */
    void executar();

    /**
     * @brief Imprime o estado atual dos registradores e flags.
     */
    void imprimirEstado();

    // A função do executor é declarada como 'amiga', o que lhe dá acesso
    // aos membros privados e protegidos da CPU (registradores, cpsr, etc.).
    friend void executarInstrucao(CPU& cpu, const Instrucao& instr);

private:
    std::array<uint32_t, 16> r; // Registradores R0-R15 (R15 é o PC)
    uint32_t cpsr;              // Current Program Status Register
    Memoria memoria;            // Instância do nosso módulo de memória

    // Verifica se a condição de uma instrução é satisfeita com base nos flags do CPSR.
    bool checarCondicao(Condicao cond);

    // Funções para manipular os bits de flag individuais no CPSR.
    bool getFlagN();
    bool getFlagZ();
    bool getFlagC();
    bool getFlagV();
    void setFlagN(bool valor);
    void setFlagZ(bool valor);
    void setFlagC(bool valor);
    void setFlagV(bool valor);
};