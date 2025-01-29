#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>

using namespace std;
using namespace chrono;

// Define um tipo para matriz (vector de vectors de doubles)
typedef vector<vector<double>> Matrix;

// Cria um gerador de números aleatórios mais robusto
std::random_device rd;
std::mt19937 generator(rd());

// Função para carregar a matriz de custos de um arquivo .csv
Matrix loadMatrixFromCSV(const string& filePath) {
    Matrix matrix;
    ifstream file(filePath);

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return matrix;
    }

    string line;
    // Lê cada linha do arquivo
    while (getline(file, line)) {
        vector<double> row;
        stringstream ss(line);
        string value;

        // Lê cada valor da linha separado por vírgula
        while (getline(ss, value, ',')) {
            try {
                row.push_back(stod(value)); // Converte o valor para double
            } catch (const invalid_argument&) {
                row.push_back(0); // Se a conversão falhar, adiciona 0
            }
        }

        matrix.push_back(row); // Adiciona a linha à matriz
    }

    file.close(); // Fecha o arquivo
    return matrix;
}

// Função para carregar os nomes das cidades de um arquivo CSV
vector<string> loadCitiesFromCSV(const string& filePath) {
    vector<string> cities;
    ifstream file(filePath);

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return cities;
    }

    string line;
    // Lê cada linha do arquivo
    while (getline(file, line)) {
        if (!line.empty()) {
            cities.push_back(line); // Adiciona a cidade ao vetor
        }
    }

    file.close(); // Fecha o arquivo
    return cities;
}

// Função para calcular o custo total de uma rota
double calculateRouteCost(const vector<int>& route, const Matrix& costMatrix) {
    double cost = 0;
    // Calcula o custo de cada trecho da rota
    for (size_t i = 0; i < route.size() - 1; ++i) {
        cost += costMatrix[route[i]][route[i + 1]];
    }
    cost += costMatrix[route.back()][route.front()]; // Adiciona o custo de voltar à cidade inicial
    return cost;
}

// Função de busca local (3-opt)
void localSearch3Opt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    // Continua enquanto houver melhorias
    while (improved) {
        improved = false;
        // Itera sobre todas as combinações possíveis de 3-opt
        for (size_t i = 0; i < route.size() - 3; ++i) {
            assert(i >= 0 && i < route.size() - 3);
            for (size_t j = i + 1; j < route.size() - 2; ++j) {
                assert(j > i && j < route.size() - 2);
                for (size_t k = j + 1; k < route.size() - 1; ++k) {
                    assert(k > j && k < route.size() - 1);

                    // Gera todas as possíveis trocas 3-opt (6 combinações no total).
                    vector<vector<int>> newRoutes;
                    vector<int> tempRoute1 = route;
                    vector<int> tempRoute2 = route;
                    vector<int> tempRoute3 = route;
                    vector<int> tempRoute4 = route;
                    vector<int> tempRoute5 = route;

                    // 1. Inverte [i+1, j]
                    reverse(tempRoute1.begin() + i + 1, tempRoute1.begin() + j + 1);
                    newRoutes.push_back(tempRoute1);

                    // 2. Inverte [j+1, k]
                    reverse(tempRoute2.begin() + j + 1, tempRoute2.begin() + k + 1);
                    newRoutes.push_back(tempRoute2);

                    // 3. Inverte [i+1, k]
                    reverse(tempRoute3.begin() + i + 1, tempRoute3.begin() + k + 1);
                    newRoutes.push_back(tempRoute3);

                    // 4. Inverte [i+1, j] e [j+1, k]
                    reverse(tempRoute4.begin() + i + 1, tempRoute4.begin() + j + 1);
                    reverse(tempRoute4.begin() + j + 1, tempRoute4.begin() + k + 1);
                    newRoutes.push_back(tempRoute4);

                    // 5. Inverte [i+1, j] e depois inverte [i+1,k]
                    reverse(tempRoute5.begin() + i + 1, tempRoute5.begin() + j + 1);
                    vector<int> tempRoute6 = tempRoute5;
                    reverse(tempRoute6.begin() + i + 1, tempRoute6.begin() + k + 1);
                    newRoutes.push_back(tempRoute6);

                    // Avalia cada nova rota
                    for (const auto& newRoute : newRoutes) {

                        double oldCost = calculateRouteCost(route, costMatrix);
                        double newCost = calculateRouteCost(newRoute, costMatrix);
                        // Se a nova rota for melhor, atualiza e sai dos laços
                        if (newCost < oldCost) {
                            route = newRoute;
                            improved = true;
                            break;
                        }
                    }
                    if (improved) break;
                }
                if (improved) break;
            }
            if (improved) break;
        }
    }
}

// Função de busca local (Or-opt)
void localSearchOrOpt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    // Continua enquanto houver melhorias
    while (improved) {
        improved = false;
         for (size_t i = 0; i < route.size(); ++i) {
            assert(i >= 0 && i < route.size());

           vector<int> newRoute = route;
            auto subPathStart = newRoute.begin() + i;
            for(int subPathSize = 1; subPathSize <=3 ; subPathSize++){
                if (i + subPathSize > route.size()) continue;
                assert(i + subPathSize <= route.size());

                auto subPathEnd = newRoute.begin() + i + subPathSize;
                assert(subPathEnd >= newRoute.begin() && subPathEnd <= newRoute.end());

                vector<int> subPath(subPathStart, subPathEnd);

                newRoute.erase(subPathStart, subPathEnd);

                 for (size_t j = 0; j < newRoute.size(); ++j) {
                    if (j == i || j == i - 1 || j == i + 1) continue;
                    assert(j >= 0 && j < newRoute.size());
                   auto newPos = newRoute.begin() + j;
                       
                       vector<int> tempRoute = newRoute;
                        tempRoute.insert(newPos, subPath.begin(), subPath.end());

                        double oldCost = calculateRouteCost(route, costMatrix);
                        double newCost = calculateRouteCost(tempRoute, costMatrix);
                         if (newCost < oldCost) {
                            route = tempRoute;
                            improved = true;
                             //After changing the vector, update the j index
                            if(j >= route.size()) j = 0;
                             break;
                         }

                 }
                if (improved) break;
            }
             if (improved) break;
        }
    }
}

// Função de construção aleatória-gulosa
vector<int> greedyRandomizedConstruction(const Matrix& costMatrix, double alpha) {
    int n = costMatrix.size();
    vector<int> route = {0}; // Começa na cidade 0
    vector<bool> visited(n, false); // Marca as cidades visitadas
    visited[0] = true; // Cidade inicial marcada como visitada

    // Enquanto houver cidades a serem visitadas
    while (route.size() < n) {
        int currentCity = route.back(); // Pega a última cidade visitada
        vector<pair<int, double>> candidates;

        // Adiciona à lista de candidatos as cidades não visitadas
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                candidates.emplace_back(i, costMatrix[currentCity][i]);
            }
        }

        // Ordena os candidatos pelo custo
        sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        // Calcula o limiar para a lista restrita de candidatos
        double minCost = candidates.front().second;
        double maxCost = candidates.back().second;
        double threshold = minCost + alpha * (maxCost - minCost);

        vector<int> restrictedCandidates;
         // Adiciona à lista restrita os candidatos cujo custo está abaixo do limiar
        for (const auto& candidate : candidates) {
            if (candidate.second <= threshold) {
                restrictedCandidates.push_back(candidate.first);
            }
        }

        assert(!restrictedCandidates.empty());
        // Escolhe uma cidade aleatoriamente da lista restrita
        uniform_int_distribution<int> distribution(0, restrictedCandidates.size() - 1);
        int chosenCity = restrictedCandidates[distribution(generator)];

        route.push_back(chosenCity);  // Adiciona a cidade à rota
        visited[chosenCity] = true; // Marca a cidade como visitada
    }

    return route;
}

// Função principal do algoritmo GRASP
pair<vector<int>, double> grasp(const Matrix& costMatrix, int maxIterations, double alpha, bool use3Opt) {
    vector<int> bestRoute;
    double bestCost = numeric_limits<double>::infinity();

    // Executa o GRASP por um número máximo de iterações
    for (int iter = 0; iter < maxIterations; ++iter) {
        // Construção aleatória-gulosa
        vector<int> route = greedyRandomizedConstruction(costMatrix, alpha);

        // Busca local
        if (use3Opt)
            localSearch3Opt(route, costMatrix);
        else
            localSearchOrOpt(route, costMatrix);

        // Avalia a solução
        double cost = calculateRouteCost(route, costMatrix);
        if (cost < bestCost) {
            bestCost = cost;
            bestRoute = route;
        }
    }

    // Adiciona o retorno para a cidade inicial na melhor rota
    bestRoute.push_back(0);
    bestCost = calculateRouteCost(bestRoute, costMatrix);

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
    // Abre o arquivo de cidades
    if (file.is_open()) {
        string line;
        // Lê cada linha do arquivo
        while (getline(file, line)) {
            if (!line.empty()) {
                cities.push_back(line);
            }
        }
        file.close(); // Fecha o arquivo
    } else {
        cerr << "Erro ao abrir o arquivo de cidades: " << citiesFile << endl;
        return 1;
    }

    // Verifica se os dados foram carregados corretamente
    if (distanceMatrix.empty() || timeMatrix.empty() || cities.empty()) {
        cerr << "Erro: Dados não carregados corretamente." << endl;
        return 1;
    }

    int maxIterations = 100; // Número máximo de iterações do GRASP
    double alpha = 0.3;  // Controle do nível de aleatoriedade

    // Aplica o GRASP para distância (usando 3-opt)
    auto start = high_resolution_clock::now();
    auto [bestRouteDist, bestCostDist] = grasp(distanceMatrix, maxIterations, alpha, true);
    auto end = high_resolution_clock::now();
    double elapsedTimeDist = duration_cast<duration<double>>(end - start).count();

    // Imprime a melhor rota para distância
     cout << "Melhor rota encontrada (Distância): ";
    for (int city : bestRouteDist) {
        cout << city << " ";
    }
    cout << "\nCusto total (Distância): " << bestCostDist << "\nTempo: " << elapsedTimeDist << "s" << endl;
/*
     // Aplica o GRASP para tempo (usando Or-opt)
    start = high_resolution_clock::now();
    auto [bestRouteTime, bestCostTime] = grasp(timeMatrix, maxIterations, alpha, false);
    end = high_resolution_clock::now();
    double elapsedTimeTime = duration_cast<duration<double>>(end - start).count();

     // Imprime a melhor rota para tempo
    cout << "Melhor rota encontrada (Tempo): ";
    for (int city : bestRouteTime) {
        cout << city << " ";
    }
    cout << "\nCusto total (Tempo): " << bestCostTime << "\nTempo: " << elapsedTimeTime << "s" << endl;
*/
    return 0;
}