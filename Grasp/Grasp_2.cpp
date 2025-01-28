#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

typedef vector<vector<double>> Matrix;

default_random_engine generator(time(0));

// Função para carregar a matriz de custos de um arquivo .csv
Matrix loadMatrixFromCSV(const string& filePath) {
    Matrix matrix;
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return matrix;
    }

    string line;
    while (getline(file, line)) {
        vector<double> row;
        stringstream ss(line);
        string value;

        while (getline(ss, value, ',')) { // Usa vírgula como separador
            try {
                row.push_back(stod(value)); // Converte o valor para double
            } catch (const invalid_argument&) {
                row.push_back(0); // Adiciona 0 caso não consiga converter
            }
        }

        matrix.push_back(row);
    }

    file.close();
    return matrix;
}

// Função para calcular o custo total da rota
double calculateRouteCost(const vector<int>& route, const Matrix& costMatrix) {
    double cost = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        cost += costMatrix[route[i]][route[i + 1]];
    }
    return cost;
}

// Estratégia de Busca Local: 2-opt
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

// Estratégia de Busca Local: Reinserção de Cidades (City Reinsertion)
void localSearchCityReinsertion(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    while (improved) {
        improved = false;

        for (size_t i = 1; i < route.size() - 1; ++i) { // Ignorar cidade inicial e final
            int cityToReinsert = route[i];
            route.erase(route.begin() + i);

            double bestIncrease = numeric_limits<double>::infinity();
            size_t bestPosition = 0;

            for (size_t j = 0; j < route.size() - 1; ++j) {
                double increase = costMatrix[route[j]][cityToReinsert] +
                                  costMatrix[cityToReinsert][route[j + 1]] -
                                  costMatrix[route[j]][route[j + 1]];

                if (increase < bestIncrease) {
                    bestIncrease = increase;
                    bestPosition = j + 1;
                }
            }

            route.insert(route.begin() + bestPosition, cityToReinsert);

            if (bestIncrease < 0) {
                improved = true;
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
pair<vector<int>, double> grasp(const Matrix& costMatrix, int maxIterations, double alpha, bool use2Opt) {
    vector<int> bestRoute;
    double bestCost = numeric_limits<double>::infinity();

    for (int iter = 0; iter < maxIterations; ++iter) {
        // Construção aleatória-gulosa
        vector<int> route = greedyRandomizedConstruction(costMatrix, alpha);

        // Busca local
        if (use2Opt) {
            localSearch2Opt(route, costMatrix);
        } else {
            localSearchCityReinsertion(route, costMatrix);
        }

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
    // Caminho para o arquivo CSV contendo a matriz de custos
    string filePath = "../matriz_1_modificado.csv";

    // Carregar a matriz de custos
    Matrix costMatrix = loadMatrixFromCSV(filePath);

    if (costMatrix.empty()) {
        cerr << "Erro: Matriz de custos não carregada." << endl;
        return 1;
    }

    int maxIterations = 100;
    double alpha = 0.3; // Controle do nível de aleatoriedade

    // GRASP com busca local 2-opt
    auto result2Opt = grasp(costMatrix, maxIterations, alpha, true);
    cout << "GRASP com 2-opt:" << endl;
    cout << "Melhor rota: ";
    for (int city : result2Opt.first) {
        cout << city << " ";
    }
    cout << "\nCusto total: " << result2Opt.second << endl;

    // GRASP com reinserção de cidades
    auto resultCityReinsertion = grasp(costMatrix, maxIterations, alpha, false);
    cout << "\nGRASP com reinserção de cidades:" << endl;
    cout << "Melhor rota: ";
    for (int city : resultCityReinsertion.first) {
        cout << city << " ";
    }
    cout << "\nCusto total: " << resultCityReinsertion.second << endl;

    return 0;
}
