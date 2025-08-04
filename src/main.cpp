#include "cpu.hpp"
#include "tuiviewer.hpp"
#include <iostream>
#include <ncurses.h>
#include <stdexcept> // ADICIONE ESTE CABEÇALHO
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso correto: " << argv[0]
              << " <caminho_para_o_arquivo.bin>\n";
    return 1;
  }

  try {
    TUIViewer tui;
    CPU meu_simulador_arm(64 * 1024);
    std::string nome_arquivo = argv[1];
    meu_simulador_arm.carregarPrograma(nome_arquivo, 0x8000);

    bool rodando = true;

    while (rodando) {
      tui.desenhar(meu_simulador_arm);
      int comando = tui.esperarInput();

      switch (comando) {
      case 'p':
        if (!meu_simulador_arm.executar()) {
          rodando = false;
        }
        break;
      case 'q':
        rodando = false;
        break;
      case 'k':
        comando = tui.esperarInput();
        tui.rolarMemoria(-4 * comando); // Rola para cima
        break;
      case 'j':
        comando = tui.esperarInput();
        tui.rolarMemoria(4 * comando); // Rola para baixo
        break;
      }
    }
  } catch (const std::exception &e) { // <-- CORREÇÃO AQUI
    // (O destrutor da TUI já foi chamado, limpando a tela do ncurses)
    std::cerr << "\nERRO FATAL DURANTE A EXECUÇÃO: " << e.what() << '\n';
    return 1;
  }

  std::cout << "Simulação encerrada pelo usuário." << std::endl;
  return 0;
}
