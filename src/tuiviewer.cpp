#include "tuiviewer.hpp"
#include <string>
#include <vector>

// Construtor: inicializa o ncurses e cria os painéis.
TUIViewer::TUIViewer() : endereco_memoria_atual(0x8000) {
  initscr();            // Inicia o modo ncurses
  cbreak();             // Desabilita buffer de linha
  noecho();             // Não exibe o caractere digitado
  curs_set(0);          // Esconde o cursor
  start_color();        // Habilita cores
  keypad(stdscr, TRUE); // Habilita teclas especiais (setas)

  // Define os pares de cores que usaremos
  init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Destaque Amarelo
  init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Destaque Ciano

  // Cria as janelas (painéis) uma única vez no construtor
  int height, width;
  getmaxyx(stdscr, height, width);

  win_borda = newwin(height, width, 0, 0);
  win_regs = newwin(18, 35, 3, 2);
  win_flags = newwin(4, 35, 21, 2);
  win_code = newwin(10, 50, 3, 38);
  win_memory = newwin(12, 50, 13, 38);
}

// Destrutor: libera a memória das janelas e restaura o terminal.
TUIViewer::~TUIViewer() {
  delwin(win_regs);
  delwin(win_flags);
  delwin(win_code);
  delwin(win_memory);
  delwin(win_borda);
  endwin();
}

int TUIViewer::esperarInput() {
  return wgetch(win_borda); // Espera input na janela principal
}

void TUIViewer::rolarMemoria(int offset) {
  if (offset < 0 && endereco_memoria_atual < (uint32_t)(-offset)) {
    endereco_memoria_atual = 0;
  } else {
    // (Adicionar verificação de limite superior aqui se necessário)
    endereco_memoria_atual += offset;
  }
}

// Método principal que chama todas as funções de desenho.
void TUIViewer::desenhar(const CPU &cpu) {
  int height, width;
  getmaxyx(stdscr, height, width);

  // Desenha a borda principal e o texto de ajuda
  werase(win_borda);
  box(win_borda, 0, 0);
  wattron(win_borda, A_BOLD);
  mvwprintw(win_borda, 1, (width - 24) / 2, "Simulador ARMv7 - PCS3732");
  wattroff(win_borda, A_BOLD);
  mvwprintw(win_borda, height - 2, 2,
            "Comandos: [p] Passo | [↑/↓] Rolar Memória | [q] Sair");
  wrefresh(win_borda);

  // Chama as funções para desenhar o conteúdo de cada painel
  desenharPainelRegistradores(cpu);
  desenharPainelFlags(cpu);
  desenharPainelCodigo(cpu);
  desenharPainelMemoria(cpu);
}

// --- Implementação dos Métodos Privados de Desenho ---

void TUIViewer::desenharPainelRegistradores(const CPU &cpu) {
  werase(win_regs);
  box(win_regs, 0, 0);
  mvwprintw(win_regs, 0, 2, " Registradores ");

  for (int i = 0; i < 16; ++i) {
    std::string reg_name = "R" + std::to_string(i);
    if (i == 13)
      reg_name = "SP (R13)";
    if (i == 14)
      reg_name = "LR (R14)";
    if (i == 15)
      reg_name = "PC (R15)";

    mvwprintw(win_regs, i + 1, 2, "%-8s: 0x%08X", reg_name.c_str(), cpu.r[i]);
  }
  wrefresh(win_regs);
}

void TUIViewer::desenharPainelFlags(const CPU &cpu) {
  werase(win_flags);
  box(win_flags, 0, 0);
  mvwprintw(win_flags, 0, 2, " Flags (CPSR) ");

  bool N = (cpu.cpsr >> 31) & 1;
  bool Z = (cpu.cpsr >> 30) & 1;
  bool C = (cpu.cpsr >> 29) & 1;
  bool V = (cpu.cpsr >> 28) & 1;

  mvwprintw(win_flags, 1, 4, "N=%d  Z=%d  C=%d  V=%d", N, Z, C, V);
  wrefresh(win_flags);
}

void TUIViewer::desenharPainelCodigo(const CPU &cpu) {
  werase(win_code);
  box(win_code, 0, 0);
  mvwprintw(win_code, 0, 2, " Código @ PC ");

  uint32_t pc = cpu.r[15];

  for (int i = -4; i <= 4; ++i) {
    uint32_t addr = pc + (i * 4);
    try {
      uint32_t instrucao = cpu.memoria.lerPalavra(addr);
      if (i == 0)
        wattron(win_code, A_REVERSE);
      mvwprintw(win_code, i + 5, 2, "> 0x%08X: 0x%08X", addr, instrucao);
      if (i == 0)
        wattroff(win_code, A_REVERSE);
    } catch (...) {
      mvwprintw(win_code, i + 5, 2, "> 0x%08X: --- (fora dos limites)", addr);
    }
  }
  wrefresh(win_code);
}

void TUIViewer::desenharPainelMemoria(const CPU &cpu) {
  werase(win_memory);
  box(win_memory, 0, 0);
  mvwprintw(win_memory, 0, 2, " Memória @ 0x%08X ",
            this->endereco_memoria_atual);

  uint32_t sp = cpu.r[13];

  for (int linha = 0; linha < 10; ++linha) {
    uint32_t addr_linha = this->endereco_memoria_atual + (linha * 4);
    try {
      if (addr_linha == (sp & 0xFFFFFFFC)) {
        wattron(win_memory, COLOR_PAIR(2));
      }

      mvwprintw(win_memory, linha + 1, 2, "0x%08X: 0x%08X", addr_linha,
                cpu.memoria.lerPalavra(addr_linha));

      if (addr_linha == (sp & 0xFFFFFFFC)) {
        wattroff(win_memory, COLOR_PAIR(2));
      }
    } catch (...) {
      mvwprintw(win_memory, linha + 1, 2, "0x%08X: --- (fora dos limites)",
                addr_linha);
    }
  }
  wrefresh(win_memory);
}
