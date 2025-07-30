#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @class Memoria
 * @brief Simula um bloco de memória RAM para o processador ARM.
 *
 * Gerencia um vetor de bytes e fornece métodos para carregar um programa
 * de um arquivo binário e para ler/escrever palavras de 32 bits,
 * tratando a ordem dos bytes (little-endian).
 */
class Memoria {
public:
  /**
   * @brief Construtor da classe Memoria.
   * @param tamanho_em_bytes O tamanho total da RAM a ser simulada.
   */
  Memoria(size_t tamanho_em_bytes);

  /**
   * @brief Carrega um arquivo binário cru (gerado pelo objcopy) na memória.
   * @param nome_arquivo O caminho para o arquivo binário.
   * @param endereco_inicial O endereço de memória onde o carregamento deve
   * começar.
   */
  void carregarDeArquivo(const std::string &nome_arquivo,
                         uint32_t endereco_inicial);

  /**
   * @brief Lê uma palavra de 32 bits da memória.
   * @param endereco O endereço de memória para ler. Deve ser alinhado em 4
   * bytes.
   * @return O valor de 32 bits lido.
   * @throws std::out_of_range se o endereço for inválido.
   */
  uint32_t lerPalavra(uint32_t endereco);

  /**
   * @brief Escreve uma palavra de 32 bits na memória.
   * @param endereco O endereço de memória para escrever. Deve ser alinhado em 4
   * bytes.
   * @param valor O valor de 32 bits a ser escrito.
   * @throws std::out_of_range se o endereço for inválido.
   */
  void escreverPalavra(uint32_t endereco, uint32_t valor);

  void printarConteudo(int start_address = 0, int end_address = -1);

private:
  // A memória é simulada como um vetor de bytes para maior flexibilidade.
  std::vector<uint8_t> ram;
  void printColHeader(int start_address, int end_address);
  void assertValidStartEndAddress(int start_address, int end_address);
  // Verifica se os endereços estão dentro dos limites da memória
};
