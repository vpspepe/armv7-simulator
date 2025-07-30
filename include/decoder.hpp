#pragma once
#include "tipos.hpp"
#include <cstdint>

/**
 * @brief Decodifica uma instrução ARM de 32 bits.
 *
 * Esta função recebe uma palavra binária de 32 bits e a traduz para uma
 * struct Instrucao, preenchendo todos os campos relevantes (opcode,
 * registradores, imediatos, etc.) para que o Executor possa entendê-la.
 *
 * @param instrucao_binaria A palavra de 32 bits lida da memória.
 * @return Uma struct Instrucao preenchida com as informações decodificadas.
 */
Instrucao decode(uint32_t instr_binaria);