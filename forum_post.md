
##  Simulador de Processador ARMv7 em C++

**Integrantes:**
* Victor Pedreira Santos Pepe
* Gabriel Silva Vilas
* Nicholas Sasaki Ogata

---

### 🎯 Objetivo Principal

O projeto visa solidificar e aplicar na prática os conceitos estudados em aula sobre a arquitetura de processadores. Através da criação de um simulador, poderemos entender em detalhes o ciclo de busca, decodificação e execução de instruções, a manipulação de registradores e o acesso à memória, que são a base do funcionamento de um processador moderno.

### 📜 Escopo e Funcionalidades (Versão Inicial)

Para garantir a viabilidade do projeto, planejamos implementar, a princípio, uma versão simplificada do processador, com as seguintes características:

* **Modelo de Execução:** Simulação de ciclo único, **sem pipeline**, onde cada instrução é executada completamente antes da próxima ser buscada.
* **Memória e Registradores:** O simulador contará com um banco de 16 registradores de 32 bits (`r0` a `r15`) e um espaço de memória RAM simulado.

#### Componentes e Instruções a serem implementadas:
* **Flags de Condição (CPSR):** O simulador irá gerenciar os quatro flags de condição principais: **N** (Negative), **Z** (Zero), **C** (Carry) e **V** (oVerflow).

* **Instruções de ULA:** Focaremos nas operações lógicas e aritméticas fundamentais:
    * `ADD` (Adição)
    * `SUB` (Subtração)
    * `AND` (E Lógico)
    * `ORR` (OU Lógico)
    * `EOR` (OU Exclusivo)
    * `MOV` (Movimentação de Dados)

* **Instruções de Acesso à Memória:** Implementaremos as instruções para carregar e salvar dados entre a memória e os registradores.
    * `LDR` (Load - 32 bits)
    * `STR` (Store - 32 bits)
    * *OBS: Inicialmente, não haverá suporte para acesso a `byte` ou `half-word`, nem para extensão de sinal.*
* **Instruções de Controle de Fluxo:** Implementaremos as instruções de salto para permitir laços e chamadas de função.
    * `B` (Branch incondicional)
    * `B<cond>` (Branches condicionais como `BEQ`, `BNE`, `BGT`, etc., que irão ler os flags do CPSR)
    * `BL` (Branch with Link, salvando no LR, para chamada de sub-rotinas)

* **Barrel Shifter:** O simulador incluirá a funcionalidade do Barrel Shifter, permitindo que o segundo operando das instruções de ULA seja deslocado (`LSL`, `LSR`, `ASR`) ou rotacionado (`ROR`) antes da operação.

### ⚙️ Como vai funcionar?

Nosso simulador seguirá o ciclo clássico de execução:
1.  **Carga:** Um programa em código de máquina ARM será carregado em nosso vetor de memória simulada através da leitura de um arquivo (ainda nao definimos se será um arquivo .txt ou um binário).
2.  **Loop de Execução (Fetch-Decode-Execute):**
    * **Fetch:** A instrução de 32 bits apontada pelo PC (`r15`) é lida da memória.
    * **Decode:** A instrução é decodificada, identificando o tipo de operação, os registradores de origem e destino, e o segundo operando.
    * **Execute:** A operação correspondente é executada, alterando o estado dos registradores ou da memória simulada.
3.  **Atualização:** O PC é incrementado e o ciclo se repete.

### 🚀 Possíveis Extensões

Caso o desenvolvimento inicial progrida bem, planejamos estender o projeto com funcionalidades adicionais, como a implementação de mais instruções.

