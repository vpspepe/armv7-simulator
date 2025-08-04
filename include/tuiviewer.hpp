#pragma once
#include "cpu.hpp"   // Inclui a definição da CPU, pois a usamos por referência
#include <ncurses.h> // Necessário para o tipo WINDOW*

/**
 * @class TUIViewer
 * @brief Gerencia a Interface de Usuário baseada em Texto (TUI) para o
 * simulador.
 *
 * Encapsula toda a lógica da biblioteca ncurses para criar um "dashboard"
 * interativo que exibe o estado da CPU em tempo real.
 */
class TUIViewer {
public:
  /**
   * @brief Construtor que inicializa o ambiente ncurses e cria os painéis.
   */
  TUIViewer();

  /**
   * @brief Destrutor que finaliza o ncurses e restaura o terminal.
   */
  ~TUIViewer();

  /**
   * @brief O método principal que desenha todos os painéis na tela.
   * @param cpu Uma referência constante para o objeto CPU a ser exibido.
   */
  void desenhar(const CPU &cpu);

  /**
   * @brief Pausa a execução e aguarda uma tecla ser pressionada pelo usuário.
   * @return O código da tecla pressionada.
   */
  int esperarInput();

  /**
   * @brief Altera o endereço base da visualização de memória.
   * @param offset O deslocamento em bytes para rolar (pode ser negativo).
   */
  void rolarMemoria(int offset);

private:
  uint32_t endereco_memoria_atual; // Endereço que o painel de memória está
                                   // visualizando

  // Ponteiros para as janelas (painéis) do ncurses
  WINDOW *win_borda;
  WINDOW *win_regs;
  WINDOW *win_flags;
  WINDOW *win_code;
  WINDOW *win_memory;

  // Métodos de desenho privados para cada painel
  void desenharPainelRegistradores(const CPU &cpu);
  void desenharPainelFlags(const CPU &cpu);
  void desenharPainelCodigo(const CPU &cpu);
  void desenharPainelMemoria(const CPU &cpu);
};
