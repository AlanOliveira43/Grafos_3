#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>

using namespace std;

typedef vector<vector<double>> Matrix;

default_random_engine generator(time(0));

// Função para calcular o custo total da rota
double calculateRouteCost(const vector<int>& route, const Matrix& costMatrix) {
    double cost = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        cost += costMatrix[route[i]][route[i + 1]];
    }
    return cost;
}

// Função de busca local (2-opt)
void localSearch2Opt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 2; ++i) {
            for (size_t j = i + 1; j < route.size() - 1; ++j) {
                vector<int> newRoute = route;
                reverse(newRoute.begin() + i, newRoute.begin() + j + 1);
                double oldCost = calculateRouteCost(route, costMatrix);
                double newCost = calculateRouteCost(newRoute, costMatrix);
                if (newCost < oldCost) {
                    route = newRoute;
                    improved = true;
                }
            }
        }
    }
}

// Função de construção aleatória e gulosa
vector<int> greedyRandomizedConstruction(const Matrix& costMatrix, double alpha) {
    int n = costMatrix.size();
    vector<int> route = {0};
    vector<bool> visited(n, false);
    visited[0] = true;

    while (route.size() < n) {
        int currentCity = route.back();
        vector<pair<int, double>> candidates;

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                candidates.emplace_back(i, costMatrix[currentCity][i]);
            }
        }

        sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        double minCost = candidates.front().second;
        double maxCost = candidates.back().second;
        double threshold = minCost + alpha * (maxCost - minCost);

        vector<int> restrictedCandidates;
        for (const auto& candidate : candidates) {
            if (candidate.second <= threshold) {
                restrictedCandidates.push_back(candidate.first);
            }
        }

        uniform_int_distribution<int> distribution(0, restrictedCandidates.size() - 1);
        int chosenCity = restrictedCandidates[distribution(generator)];

        route.push_back(chosenCity);
        visited[chosenCity] = true;
    }

    route.push_back(0); // Voltar para a cidade inicial
    return route;
}

// Função principal do algoritmo GRASP
pair<vector<int>, double> grasp(const Matrix& costMatrix, int maxIterations, double alpha) {
    vector<int> bestRoute;
    double bestCost = numeric_limits<double>::infinity();

    for (int iter = 0; iter < maxIterations; ++iter) {
        // Construção aleatória-gulosa
        vector<int> route = greedyRandomizedConstruction(costMatrix, alpha);

        // Busca local (2-opt)
        localSearch2Opt(route, costMatrix);

        // Avaliar a solução
        double cost = calculateRouteCost(route, costMatrix);
        if (cost < bestCost) {
            bestCost = cost;
            bestRoute = route;
        }
    }

    return {bestRoute, bestCost};
}

// Função principal para testar o algoritmo GRASP
int main() {
    // Exemplo de matriz de custo
    Matrix costMatrix = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };

    int maxIterations = 100;
    double alpha = 0.3; // Controle do nível de aleatoriedade

    auto result = grasp(costMatrix, maxIterations, alpha);

    cout << "Melhor rota encontrada: ";
    for (int city : result.first) {
        cout << city << " ";
    }
    cout << endl;

    cout << "Custo total da rota: " << result.second << endl;

    return 0;
}
