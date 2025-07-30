#include "memoria.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream> // Para std::ifstream (leitura de arquivos)
#include <iomanip>
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

void Memoria::printarConteudo(int start_address, int end_address) {
  if (end_address == -1)
    end_address = ram.size();
  // Assertions para garantir que os endereços estão dentro dos limites
  assertValidStartEndAddress(start_address, end_address);
  // Trackear os endereços printados
  size_t address = start_address;
  printColHeader(start_address, end_address);
  std::cout << std::hex << std::uppercase << std::setw(8) << std::setfill('0')
            << address << ": ";
  // Iterar sobre valores da memória RAM
  for (size_t i = start_address; i < size_t(end_address); ++i) {
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
              << static_cast<int>(ram[i]) << " ";
    address++;
    // A cada 16 bytes, pula para a próxima linha e imprime o endereço
    // correspondente.
    if ((i + 1) % 16 == 0) {
      std::cout << std::endl;
      std::cout << std::hex << std::uppercase << std::setw(8)
                << std::setfill('0') << address << ": ";
    }
  }
  std::cout << std::endl;
}

void Memoria::printColHeader(int start_address, int end_address) {
  std::cout << "Conteúdo da memória de " << std::hex << std::uppercase
            << std::setw(8) << std::setfill('0') << start_address << " até "
            << std::hex << std::uppercase << std::setw(8) << std::setfill('0')
            << end_address - 1 << ":" << std::endl;
  std::cout << "Endereços: ";
  // Imprimir colunas de endereços: 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  std::cout << std::endl << "          ";
  for (int i = 0; i < 16; ++i) {
    std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
              << i << " ";
  }
  std::cout << std::endl;
}

void Memoria::assertValidStartEndAddress(int start_address, int end_address) {
  assert(("start_address deve ser >= 0", start_address >= 0));
  assert(("end_address deve ser <= tamanho da RAM", end_address <= ram.size()));
  assert(("start_address deve ser < end_address", start_address < end_address));
}
