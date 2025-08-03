#include "tuiviewer.hpp"
#include "cpu.hpp" // Inclui a definição completa da CPU
#include <ncurses.h>
#include <string>
#include <vector>

// --- Declarações de Funções Auxiliares Internas ---
namespace {
void desenharPainelRegistradores(WINDOW *win, const CPU &cpu);
void desenharPainelFlags(WINDOW *win, const CPU &cpu);
void desenharPainelCodigo(WINDOW *win, const CPU &cpu);
void desenharPainelMemoria(WINDOW *win, const CPU &cpu);
} // namespace

// Construtor: inicializa o ncurses.
TUIViewer::TUIViewer() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  start_color();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
}

// Destrutor: restaura o terminal.
TUIViewer::~TUIViewer() { endwin(); }

int TUIViewer::esperarInput() { return getch(); }

// O método principal que desenha todo o "dashboard".
void TUIViewer::desenhar(const CPU &cpu) {
  int height, width;
  getmaxyx(stdscr, height, width);

  clear();

  box(stdscr, 0, 0);
  attron(A_BOLD);
  mvprintw(1, (width - 24) / 2, "Simulador ARMv7 - PCS3732");
  attroff(A_BOLD);
  mvprintw(height - 2, 2, "Comandos: [s] Step | [q] Quit");
  refresh();

  WINDOW *reg_win = newwin(18, 35, 3, 2);
  WINDOW *flag_win = newwin(4, 35, 21, 2);
  WINDOW *code_win = newwin(10, 50, 3, 38);
  WINDOW *mem_win = newwin(12, 50, 13, 38);

  desenharPainelRegistradores(reg_win, cpu);
  desenharPainelFlags(flag_win, cpu);
  desenharPainelCodigo(code_win, cpu);
  desenharPainelMemoria(mem_win, cpu);

  delwin(reg_win);
  delwin(flag_win);
  delwin(code_win);
  delwin(mem_win);
}

// --- Implementação das Funções Auxiliares de Desenho ---
namespace {
void desenharPainelRegistradores(WINDOW *win, const CPU &cpu) {
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Registradores ");

  for (int i = 0; i < 16; ++i) {
    std::string reg_name = "R" + std::to_string(i);
    if (i == 13)
      reg_name = "SP (R13)";
    if (i == 14)
      reg_name = "LR (R14)";
    if (i == 15)
      reg_name = "PC (R15)";

    // Acesso direto ao membro privado 'r', permitido pela declaração 'friend'.
    mvwprintw(win, i + 1, 2, "%-8s: 0x%08X", reg_name.c_str(), cpu.r[i]);
  }
  wrefresh(win);
}

void desenharPainelFlags(WINDOW *win, const CPU &cpu) {
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Flags (CPSR) ");

  // Acesso direto ao membro privado 'cpsr' para extrair os flags.
  bool N = (cpu.cpsr >> 31) & 1;
  bool Z = (cpu.cpsr >> 30) & 1;
  bool C = (cpu.cpsr >> 29) & 1;
  bool V = (cpu.cpsr >> 28) & 1;

  mvwprintw(win, 1, 4, "N=%d  Z=%d  C=%d  V=%d", N, Z, C, V);
  wrefresh(win);
}

void desenharPainelCodigo(WINDOW *win, const CPU &cpu) {
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Código @ PC ");

  // Acesso direto a r[15] para pegar o PC.
  uint32_t pc = cpu.r[15];

  for (int i = -4; i <= 4; ++i) {
    uint32_t addr = pc + (i * 4);

    try {
      // Acesso direto ao membro 'memoria' e seu método público.
      uint32_t instrucao = cpu.memoria.lerPalavra(addr);
      if (i == 0) {
        wattron(win, A_REVERSE);
      }

      mvwprintw(win, i + 5, 2, "> 0x%08X: 0x%08X", addr, instrucao);

      if (i == 0) {
        wattroff(win, A_REVERSE);
      }
    } catch (const std::exception &e) {
      // Endereço inválido, não desenha nada para aquela linha
    }
  }
  wrefresh(win);
}

void desenharPainelMemoria(WINDOW *win, const CPU &cpu) {
  box(win, 0, 0);
  // Acesso direto a r[13] para pegar o SP.
  uint32_t sp = cpu.r[13];
  uint32_t base_addr = (sp > 16) ? (sp - 16) & 0xFFFFFFF0 : 0;

  mvwprintw(win, 0, 2, " Memória @ SP (0x%08X) ", sp);

  for (int linha = 0; linha < 10; ++linha) {
    uint32_t addr_linha = base_addr + (linha * 4);
    // Assumindo que a classe Memoria tem um método público getTamanho()
    // if (addr_linha >= cpu.memoria.getTamanho()) break;

    try {
      if (addr_linha == (sp & 0xFFFFFFFC)) {
        wattron(win, COLOR_PAIR(2));
      }

      // Acesso direto ao membro 'memoria' e seu método público.
      mvwprintw(win, linha + 1, 2, "0x%08X: 0x%08X", addr_linha,
                cpu.memoria.lerPalavra(addr_linha));

      if (addr_linha == (sp & 0xFFFFFFFC)) {
        wattroff(win, COLOR_PAIR(2));
      }
    } catch (const std::exception &e) {
      // Endereço inválido, não desenha
    }
  }
  wrefresh(win);
}
} // namespace
