// Em tuiviewer.hpp
#pragma once
#include "cpu.hpp"

class TUIViewer {
public:
  TUIViewer();  // No construtor, você chama initscr() e configura o ncurses
  ~TUIViewer(); // No destrutor, você chama endwin() para restaurar o terminal

  void desenhar(const CPU &cpu); // Desenha todos os painéis
  int esperarInput();            // Usa getch() para esperar uma tecla
};
