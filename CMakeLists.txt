# Versão mínima do CMake necessária para este script.
cmake_minimum_required(VERSION 3.10)

# Define o nome do projeto e a linguagem principal (C++).
project(
  ARMv7_Simulator
  VERSION 1.0
  LANGUAGES CXX)

# --- Configurações do Compilador ---

# Define o padrão C++ a ser utilizado (C++17 é uma boa escolha moderna).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adiciona flags de compilação para todos os alvos do projeto. -Wall e -Wextra:
# Ativam um conjunto robusto de avisos (warnings). -pedantic: Garante
# conformidade com o padrão C++. -g: Inclui informações de depuração no
# executável. -O2: Um bom nível de otimização para a versão final (pode ser -O0
# para depurar).
add_compile_options(-Wall -Wextra -pedantic -g -O2)

# --- Definição do Executável ---

# Define o nome do nosso programa executável final.
set(EXECUTABLE_NAME arm_simulator)

# Encontra todos os arquivos de implementação (.cpp) na pasta 'src'. O CMake irá
# compilá-los automaticamente.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Cria o alvo executável a partir dos arquivos-fonte encontrados.
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# --- Configuração de Caminhos e Bibliotecas ---

# Informa ao compilador para procurar por arquivos de cabeçalho (#include) na
# nossa pasta 'include'. PUBLIC significa que qualquer outro projeto que linkar
# com o nosso também terá acesso a esta pasta.
target_include_directories(${EXECUTABLE_NAME} PUBLIC include)

# Se você tivesse bibliotecas externas para linkar, usaria:
# target_link_libraries(${EXECUTABLE_NAME} PUBLIC nome_da_biblioteca)

# --- Organização dos Arquivos de Saída (Opcional, mas recomendado) ---

# Define que os executáveis finais devem ser colocados na pasta 'bin' dentro do
# diretório de compilação (ex: build/bin/).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Mensagens de Status ---

# Imprime uma mensagem útil ao final da configuração do CMake.
message(STATUS "Projeto configurado com sucesso!")
message(STATUS "Para compilar, execute: make")
message(STATUS "O executável será gerado em: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
