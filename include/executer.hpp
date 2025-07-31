#pragma once
#include "tipos.h" // Precisa conhecer a struct Instrucao

// Declaração avançada: diz ao compilador "existe uma classe chamada CPU",
// sem precisar de todos os detalhes dela.
class CPU;

/**
 * @brief Executa uma única instrução decodificada.
 *
 * Esta função é o ponto de entrada do módulo executor. Ela recebe o estado
 * atual da CPU (por referência, para que possa ser modificado) e a instrução
 * a ser executada.
 *
 * @param cpu Referência para o objeto CPU, que contém o estado (registradores, etc.).
 * @param instr A instrução já decodificada pelo módulo Decodificador.
 */
void executarInstrucao(CPU& cpu, const Instrucao& instr);