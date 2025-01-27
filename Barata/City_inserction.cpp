#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

typedef vector<vector<double>> Matrix;

default_random_engine generator;

pair<vector<int>, double> insercaoMaisBarataCityInsertion(const Matrix& costMatrix) {
    int n = costMatrix.size(); // Número de cidades

    // Passo 1: Começar com duas cidades arbitrárias
    vector<int> route = {0, 1, 0}; // Começar com a cidade 0 e 1 e voltar à 0
    vector<bool> visited(n, false);
    visited[0] = true;
    visited[1] = true;

    // Passo 2: Inserir cidades restantes iterativamente na posição de menor custo
    while (route.size() < n + 1) {
        double bestIncrease = numeric_limits<double>::infinity();
        int bestCity = -1;
        int bestPosition = -1;

        // Avaliar todas as cidades não visitadas
        for (int city = 0; city < n; ++city) {
            if (visited[city]) continue;

            // Testar inserção da cidade em todas as posições possíveis no percurso
            for (int pos = 0; pos < route.size() - 1; ++pos) {
                double increase = costMatrix[route[pos]][city] +
                                 costMatrix[city][route[pos + 1]] -
                                 costMatrix[route[pos]][route[pos + 1]];

                // Atualizar melhor inserção encontrada
                if (increase < bestIncrease) {
                    bestIncrease = increase;
                    bestCity = city;
                    bestPosition = pos;
                }
            }
        }

        // Inserir a melhor cidade encontrada na melhor posição
        route.insert(route.begin() + bestPosition + 1, bestCity);
        visited[bestCity] = true;
    }

    // Passo 3: Otimizar com Inserção de Cidades (City Insertion)
    bool improved = true;
    while (improved) {
        improved = false;

        for (size_t i = 1; i < route.size() - 2; ++i) { // Não considerar cidade inicial e final
            int cityToMove = route[i];
            route.erase(route.begin() + i); // Remover a cidade temporariamente

            double bestLocalIncrease = numeric_limits<double>::infinity();
            int bestLocalPosition = -1;

            // Avaliar todas as novas posições para inserir a cidade
            for (size_t pos = 0; pos < route.size() - 1; ++pos) {
                double increase = costMatrix[route[pos]][cityToMove] +
                                 costMatrix[cityToMove][route[pos + 1]] -
                                 costMatrix[route[pos]][route[pos + 1]];

                if (increase < bestLocalIncrease) {
                    bestLocalIncrease = increase;
                    bestLocalPosition = pos;
                }
            }

            // Se encontrar uma posição melhor, atualizar a rota
            if (bestLocalIncrease < 0) {
                route.insert(route.begin() + bestLocalPosition + 1, cityToMove);
                improved = true;
                break;
            } else {
                route.insert(route.begin() + i, cityToMove);
            }
        }
    }

    // Passo 4: Calcular o custo total da rota final
    double bestCost = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        bestCost += costMatrix[route[i]][route[i + 1]];
    }

    return {route, bestCost};
}

// Função principal para testar o algoritmo
int main() {
    // Exemplo de matriz de custo
    Matrix costMatrix = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };

    auto result = insercaoMaisBarataCityInsertion(costMatrix);

    cout << "Melhor rota encontrada: ";
    for (int city : result.first) {
        cout << city << " ";
    }
    cout << endl;

    cout << "Custo total da rota: " << result.second << endl;

    return 0;
}
