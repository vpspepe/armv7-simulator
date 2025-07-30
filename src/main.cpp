#include "memoria.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::string nome_arquivo = argv[1];
  std::cout << "Carregando arquivo: " << nome_arquivo << std::endl;
  Memoria memoria(60 * 16); // 1 MB de memória RAM simulada
  // Carrega o arquivo binário na memória
  memoria.carregarDeArquivo(nome_arquivo, 0x00000000);
  // Lê uma palavra de 32 bits da memória
  uint32_t valor_lido = memoria.lerPalavra(0x00000004);
  memoria.escreverPalavra(0x00000004, 0x12345678);
  uint32_t valor_escrito = memoria.lerPalavra(0x00000004);
  std::cout << "Valor lido em 0x4: " << std::hex << valor_lido << std::endl;
  std::cout << "Valor escrito em 0x4: " << std::hex << valor_escrito
            << std::endl;
  memoria.printarConteudo(0, 48);
  return 0;
}
