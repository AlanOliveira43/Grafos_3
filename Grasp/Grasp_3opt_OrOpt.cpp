#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;

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

// Função para carregar os nomes das cidades de um arquivo CSV
vector<string> loadCitiesFromCSV(const string& filePath) {
    vector<string> cities;
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return cities;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            cities.push_back(line);
        }
    }

    file.close();
    return cities;
}

// Função para calcular o custo total de uma rota
double calculateRouteCost(const vector<int>& route, const Matrix& costMatrix) {
    double cost = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        cost += costMatrix[route[i]][route[i + 1]];
    }
    // Adiciona o custo de voltar ao ponto inicial
    cost += costMatrix[route.back()][route.front()];
    return cost;
}

// Função de busca local (3-opt)
void localSearch3Opt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 3; ++i) {
            for (size_t j = i + 1; j < route.size() - 2; ++j) {
                for (size_t k = j + 1; k < route.size() - 1; ++k) {
                    // Gera todas as possíveis combinações de 3-opt
                    vector<vector<int>> newRoutes = {
                        route,
                        route
                    };
                    reverse(newRoutes[1].begin() + i, newRoutes[1].begin() + j + 1);
                    reverse(newRoutes[2].begin() + j, newRoutes[2].begin() + k + 1);

                    // Avalia cada nova rota
                    for (const auto& newRoute : newRoutes) {
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
    }
}

// Função de busca local (Or-opt)
void localSearchOrOpt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 1; ++i) {
            for (size_t j = 1; j < route.size(); ++j) {
                if (j == i || j == i - 1) continue;

                // Move um subcaminho de tamanho 1, 2 ou 3 para uma nova posição
                vector<int> newRoute = route;
                auto subPath = newRoute.begin() + i;
                auto newPos = newRoute.begin() + j;
                newRoute.insert(newPos, subPath, subPath + 1);
                newRoute.erase(subPath);

                // Avalia a nova rota
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

// Função de construção aleatória-gulosa
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

        // Busca local (3-opt ou Or-opt)
        localSearch3Opt(route, costMatrix); // Ou localSearchOrOpt(route, costMatrix);

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
    // Caminhos dos arquivos
    string distanceFile = "../Km_modificado.csv";
    string timeFile = "../Min_modificado.csv";
    string citiesFile = "../Cidades.csv";
    string outputFile = "../resultados_swap.csv";

    // Carregar a matriz de distâncias
    cout << "Carregando a matriz de distâncias..." << endl;
    Matrix distanceMatrix = loadMatrixFromCSV(distanceFile);

    // Carregar a matriz de tempos
    cout << "Carregando a matriz de tempos..." << endl;
    Matrix timeMatrix = loadMatrixFromCSV(timeFile);

    // Carregar os nomes das cidades
    cout << "Carregando os nomes das cidades..." << endl;
    vector<string> cities;
    ifstream file(citiesFile);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                cities.push_back(line);
            }
        }
        file.close();
    } else {
        cerr << "Erro ao abrir o arquivo de cidades: " << citiesFile << endl;
        return 1;
    }

    if (distanceMatrix.empty() || timeMatrix.empty() || cities.empty()) {
        cerr << "Erro: Dados não carregados corretamente." << endl;
        return 1;
    }

    int maxIterations = 100;
    double alpha = 0.3; // Controle do nível de aleatoriedade

    // Aplicar GRASP para distância
    auto start = high_resolution_clock::now();
    auto [bestRouteDist, bestCostDist] = grasp(distanceMatrix, maxIterations, alpha);
    auto end = high_resolution_clock::now();
    double elapsedTimeDist = duration_cast<duration<double>>(end - start).count();

    cout << "Melhor rota encontrada (Distância): ";
    for (int city : bestRouteDist) {
        cout << city << " ";
    }
    cout << "\nCusto total (Distância): " << bestCostDist << "\nTempo: " << elapsedTimeDist << "s" << endl;

    // Aplicar GRASP para tempo
    start = high_resolution_clock::now();
    auto [bestRouteTime, bestCostTime] = grasp(timeMatrix, maxIterations, alpha);
    end = high_resolution_clock::now();
    double elapsedTimeTime = duration_cast<duration<double>>(end - start).count();

    cout << "Melhor rota encontrada (Tempo): ";
    for (int city : bestRouteTime) {
        cout << city << " ";
    }
    cout << "\nCusto total (Tempo): " << bestCostTime << "\nTempo: " << elapsedTimeTime << "s" << endl;

    return 0;
}
