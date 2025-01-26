#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

// Função para calcular o custo total de uma rota
double calcularCusto(const vector<vector<double>>& matriz, const vector<int>& rota) {
    double custo = 0;
    for (int i = 0; i < rota.size() - 1; ++i) {
        custo += matriz[rota[i]][rota[i + 1]];
    }
    custo += matriz[rota.back()][rota.front()]; // Retorno à cidade inicial
    return custo;
}

// Função de busca local: Swap (Troca de Vizinhos)
void buscaLocalSwap(const vector<vector<double>>& matriz, vector<int>& rota) {
    int n = rota.size();
    double melhorCusto = calcularCusto(matriz, rota);

    for (int i = 1; i < n - 1; ++i) {
        for (int j = i + 1; j < n - 1; ++j) {
            swap(rota[i], rota[j]);
            double novoCusto = calcularCusto(matriz, rota);
            if (novoCusto < melhorCusto) {
                melhorCusto = novoCusto;
            } else {
                swap(rota[i], rota[j]); // Desfaz a troca
            }
        }
    }
}

// Função de busca local: 2-opt
void buscaLocal2Opt(const vector<vector<double>>& matriz, vector<int>& rota) {
    int n = rota.size();
    double melhorCusto = calcularCusto(matriz, rota);

    for (int i = 1; i < n - 2; ++i) {
        for (int j = i + 1; j < n - 1; ++j) {
            reverse(rota.begin() + i, rota.begin() + j + 1);
            double novoCusto = calcularCusto(matriz, rota);
            if (novoCusto < melhorCusto) {
                melhorCusto = novoCusto;
            } else {
                reverse(rota.begin() + i, rota.begin() + j + 1); // Desfaz a reversão
            }
        }
    }
}

// Algoritmo GRASP
vector<int> grasp(const vector<vector<double>>& matriz, int cidadeInicial, int maxIteracoes) {
    int n = matriz.size();
    vector<int> melhorRota;
    double melhorCusto = numeric_limits<double>::max();

    for (int iteracao = 0; iteracao < maxIteracoes; ++iteracao) {
        vector<int> rota;
        vector<bool> visitado(n, false);

        // Gera uma solução inicial gulosa randomizada
        rota.push_back(cidadeInicial);
        visitado[cidadeInicial] = true;

        while (rota.size() < n) {
            vector<pair<int, double>> candidatos;
            int cidadeAtual = rota.back();

            for (int i = 0; i < n; ++i) {
                if (!visitado[i]) {
                    candidatos.emplace_back(i, matriz[cidadeAtual][i]);
                }
            }

            // Ordena candidatos pelo custo
            sort(candidatos.begin(), candidatos.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
                return a.second < b.second;
            });

            // Escolhe aleatoriamente entre os melhores candidatos (aleatoriedade controlada)
            int limite = max(1, (int)(candidatos.size() * 0.2)); // Top 20%
            int escolhido = rand() % limite;

            rota.push_back(candidatos[escolhido].first);
            visitado[candidatos[escolhido].first] = true;
        }

        // Busca local
        buscaLocalSwap(matriz, rota);
        buscaLocal2Opt(matriz, rota);

        // Atualiza a melhor solução encontrada
        double custoAtual = calcularCusto(matriz, rota);
        if (custoAtual < melhorCusto) {
            melhorCusto = custoAtual;
            melhorRota = rota;
        }
    }

    return melhorRota;
}

int main() {
    srand(time(0));

    // Exemplo de matriz de custos
    vector<vector<double>> matriz = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };

    int cidadeInicial = 0; // Cidade inicial
    int maxIteracoes = 100; // Número de iterações do GRASP

    vector<int> melhorRota = grasp(matriz, cidadeInicial, maxIteracoes);
    double melhorCusto = calcularCusto(matriz, melhorRota);

    cout << "Melhor Rota (GRASP + Busca Local): ";
    for (int cidade : melhorRota) {
        cout << cidade << " ";
    }
    cout << endl;
    cout << "Custo Total: " << melhorCusto << endl;

    return 0;
}
