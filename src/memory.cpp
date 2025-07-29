#include "memoria.hpp"
#include <fstream>   // Para std::ifstream (leitura de arquivos)
#include <iostream>  // Para std::cerr (saída de erro)
#include <stdexcept> // Para std::runtime_error e std::out_of_range

/**
 * @brief Construtor da classe Memoria.
 *
 * Utiliza a lista de inicialização do C++ para criar o vetor 'ram'
 * com o tamanho especificado e preenchê-lo com zeros.
 */
Memoria::Memoria(size_t tamanho_em_bytes) : ram(tamanho_em_bytes, 0) {}

/**
 * @brief Carrega um arquivo binário cru na memória.
 *
 * Abre um arquivo em modo binário e copia seu conteúdo diretamente
 * para o vetor 'ram', a partir de um endereço inicial.
 */
void Memoria::carregarDeArquivo(const std::string &nome_arquivo,
                                uint32_t endereco_inicial) {
  std::ifstream arquivo(nome_arquivo, std::ios::binary | std::ios::ate);

  if (!arquivo) {
    throw std::runtime_error(
        "Erro: Não foi possível abrir o arquivo binário: " + nome_arquivo);
  }

  std::streamsize tamanho_arquivo = arquivo.tellg();
  arquivo.seekg(0, std::ios::beg);

  if (endereco_inicial + tamanho_arquivo > ram.size()) {
    throw std::runtime_error(
        "Erro: O arquivo é grande demais para a memória simulada.");
  }

  // Lê o conteúdo do arquivo diretamente para o vetor 'ram' na posição correta.
  if (!arquivo.read(reinterpret_cast<char *>(&ram[endereco_inicial]),
                    tamanho_arquivo)) {
    throw std::runtime_error("Erro ao ler o conteúdo do arquivo: " +
                             nome_arquivo);
  }
}

/**
 * @brief Lê uma palavra de 32 bits da memória.
 *
 * Realiza verificações de alinhamento e limites antes de ler 4 bytes
 * e combiná-los em um inteiro de 32 bits, respeitando a ordem little-endian.
 */
uint32_t Memoria::lerPalavra(uint32_t endereco) {
  // Validação: Garante que o endereço é alinhado em 4 bytes.
  if (endereco % 4 != 0) {
    // Em um processador real, isso geraria uma exceção de Data Abort.
    throw std::runtime_error(
        "Erro de alinhamento: Tentativa de leitura em endereço não alinhado: " +
        std::to_string(endereco));
  }

  // Validação: Garante que o endereço está dentro dos limites da memória.
  if (endereco + 3 >= ram.size()) {
    throw std::out_of_range("Erro de segmentação: Tentativa de leitura fora "
                            "dos limites da memória no endereço " +
                            std::to_string(endereco));
  }

  // Combina os 4 bytes em uma palavra de 32 bits (Little-Endian)
  // O byte no endereço mais baixo é o menos significativo.
  uint32_t byte0 = ram[endereco];
  uint32_t byte1 = ram[endereco + 1];
  uint32_t byte2 = ram[endereco + 2];
  uint32_t byte3 = ram[endereco + 3];

  return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

/**
 * @brief Escreve uma palavra de 32 bits na memória.
 *
 * Realiza verificações de alinhamento e limites antes de "quebrar"
 * um inteiro de 32 bits em 4 bytes e escrevê-los na memória.
 */
void Memoria::escreverPalavra(uint32_t endereco, uint32_t valor) {
  // Validação: Garante que o endereço é alinhado em 4 bytes.
  if (endereco % 4 != 0) {
    throw std::runtime_error(
        "Erro de alinhamento: Tentativa de escrita em endereço não alinhado: " +
        std::to_string(endereco));
  }

  // Validação: Garante que o endereço está dentro dos limites da memória.
  if (endereco + 3 >= ram.size()) {
    throw std::out_of_range("Erro de segmentação: Tentativa de escrita fora "
                            "dos limites da memória no endereço " +
                            std::to_string(endereco));
  }

  // Quebra a palavra de 32 bits em 4 bytes (Little-Endian)
  // O byte menos significativo é armazenado no endereço mais baixo.
  ram[endereco] = valor & 0xFF;
  ram[endereco + 1] = (valor >> 8) & 0xFF;
  ram[endereco + 2] = (valor >> 16) & 0xFF;
  ram[endereco + 3] = (valor >> 24) & 0xFF;
}
