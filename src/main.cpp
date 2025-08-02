#include "cpu.hpp"
#include <iostream>
#include <string>

/**
 * @brief Ponto de entrada principal do simulador ARM.
 *
 * Responsável por:
 * 1. Validar os argumentos da linha de comando.
 * 2. Instanciar a CPU com um tamanho de memória definido.
 * 3. Carregar o programa binário na memória da CPU.
 * 4. Iniciar o ciclo de execução.
 * 5. Imprimir o estado final dos registradores.
 * 6. Capturar e reportar quaisquer erros que ocorram durante a simulação.
 */
int main(int argc, char* argv[]) {
    // O programa espera receber exatamente um argumento: o caminho para o arquivo binário.
    if (argc != 2) {
        std::cerr << "Uso correto: " << argv[0] << " <caminho_para_o_arquivo.bin>\\n";
        return 1; // Retorna um código de erro
    }

    std::string nome_arquivo = argv[1];

    try {
        // Cria uma instância da nossa CPU com 64KB de RAM simulada.
        CPU meu_simulador_arm(64 * 1024);

        // Carrega o arquivo binário fornecido no endereço de carga padrão (0x8000).
        std::cout << "Carregando programa '" << nome_arquivo << "'...\\n";
        meu_simulador_arm.carregarPrograma(nome_arquivo, 0x8000);

        // Execução da Simulação
        std::cout << "Iniciando simulação...\\n";
        meu_simulador_arm.executar();

        // Estado Final dos registradores
        std::cout << "\\nSimulação concluída. Estado final dos registradores:\\n";
        meu_simulador_arm.imprimirEstado();

    } catch (const std::exception& e) {
        // Se qualquer parte da simulação (leitura de arquivo, acesso à memória,
        // instrução inválida) lançar uma exceção, ela será capturada aqui.
        std::cerr << "\\nERRO FATAL DURANTE A EXECUÇÃO: " << e.what() << '\\n';
        return 1; // Retorna um código de erro
    }

    return 0;
}
