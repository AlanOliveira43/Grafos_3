# Projeto: Problema do Caixeiro Viajante

## Descrição do Projeto

Este projeto implementa diferentes algoritmos para resolver o Problema do Caixeiro Viajante (TSP). Os algoritmos implementados são:

1. Algoritmo Guloso
2. Algoritmo da Inserção Mais Barata
3. Algoritmo GRASP com Busca Local (3-opt e Swap)

Os dados utilizados são descritos no TCC de Abdiel, contendo 12 problemas (6 por distância e 6 por tempo). Os resultados obtidos serão comparados com soluções exatas fornecidas pela coluna GLPK na Tabela 5 do TCC.

## Como Executar

### Pré-requisitos

- Compilador C++ (g++)
- Sistema operacional Linux

### Passos para Compilar e Executar

1. Clone o repositório:
    ```bash
    git clone git@github.com:AlanOliveira43/Grafos_3.git
    cd Grafos_3
    ```

2. Entre em cada pasta e compile os arquivos:
    ```bash
    cd Greedy
    g++ -o guloso Guloso2.cpp
    g++ -o teste Teste2.cpp
    cd ../Grasp
    g++ -o grasp2 Grasp_2.cpp
    g++ -o grasp3opt Grasp_3opt_OrOpt.cpp
    cd ../Barata
    g++ -o teste Teste.cpp
    g++ -o subcaminho Subcaminho2.cpp
    g++ -o city City2.cpp
    cd ..
    ```

3. Entre em cada pasta execute os binários:
    ```bash
    cd Greedy
    ./guloso
    ./teste
    cd ../Grasp
    ./grasp2
    ./grasp3opt
    cd ../Barata
    ./teste
    ./subcaminho
    ./city
    cd ..
    ```
## Contribuidores
- Alan de Castro Oliveira
- George Antonio dos Santos Bezerra
- Jorge William Câmara Sales