#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// Função para calcular o custo total de um percurso
double calculatePathCost(const vector<int>& path, const vector<vector<double>>& distanceMatrix) {
    double cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += distanceMatrix[path[i]][path[i + 1]];
    }
    return cost;
}

// Algoritmo Guloso para encontrar uma solução inicial
pair<vector<int>, double> greedyTSP(const vector<vector<double>>& distanceMatrix) {
    size_t numCities = distanceMatrix.size();
    vector<bool> visited(numCities, false);
    vector<int> path;
    int currentCity = 0;
    visited[currentCity] = true;
    path.push_back(currentCity);
    double totalCost = 0;

    for (size_t i = 1; i < numCities; ++i) {
        double minDistance = numeric_limits<double>::infinity();
        int nextCity = -1;

        for (size_t j = 0; j < numCities; ++j) {
            if (!visited[j] && distanceMatrix[currentCity][j] < minDistance) {
                minDistance = distanceMatrix[currentCity][j];
                nextCity = j;
            }
        }

        path.push_back(nextCity);
        visited[nextCity] = true;
        totalCost += minDistance;
        currentCity = nextCity;
    }

    // Retorna à cidade inicial
    totalCost += distanceMatrix[currentCity][path[0]];
    path.push_back(path[0]);

    return {path, totalCost};
}

// Método de Troca de Vizinhos (Swap)
pair<vector<int>, double> swapNeighbors(const vector<int>& initialPath, const vector<vector<double>>& distanceMatrix) {
    vector<int> bestPath = initialPath;
    double bestCost = calculatePathCost(bestPath, distanceMatrix);
    bool improved = true;

    while (improved) {
        improved = false;
        for (size_t i = 1; i < bestPath.size() - 2; ++i) {
            for (size_t j = i + 1; j < bestPath.size() - 1; ++j) {
                vector<int> newPath = bestPath;
                swap(newPath[i], newPath[j]);
                double newCost = calculatePathCost(newPath, distanceMatrix);

                if (newCost < bestCost) {
                    bestPath = newPath;
                    bestCost = newCost;
                    improved = true;
                }
            }
        }
    }

    return {bestPath, bestCost};
}

int main() {
    // Exemplo de matriz de distâncias
    vector<vector<double>> distanceMatrix = {
        {0, 38.8, 29.7, 37.4},
        {38.8, 0, 31.5, 39.3},
        {29.7, 31.5, 0, 9.9},
        {37.4, 39.3, 9.9, 0}
    };

    // Aplicar o Algoritmo Guloso
    auto [greedyPath, greedyCost] = greedyTSP(distanceMatrix);

    cout << "Percurso inicial (Guloso): ";
    for (int city : greedyPath) {
        cout << city << " ";
    }
    cout << "\nCusto inicial: " << greedyCost << "\n";

    // Aplicar o método de Troca de Vizinhos
    auto [optimizedPath, optimizedCost] = swapNeighbors(greedyPath, distanceMatrix);

    cout << "Percurso otimizado (Swap): ";
    for (int city : optimizedPath) {
        cout << city << " ";
    }
    cout << "\nCusto otimizado: " << optimizedCost << "\n";

    return 0;
}
