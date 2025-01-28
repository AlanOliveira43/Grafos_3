#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

typedef vector<vector<double>> Matrix;

// Função para carregar a matriz de custos a partir de um arquivo CSV
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

// Função para calcular o custo total de um percurso
double calculatePathCost(const vector<int>& path, const Matrix& costMatrix) {
    double cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += costMatrix[path[i]][path[i + 1]];
    }
    return cost;
}

// Algoritmo da Inserção Mais Barata
pair<vector<int>, double> insercaoMaisBarata(const Matrix& costMatrix) {
    int n = costMatrix.size(); // Número de cidades

    // Começar com duas cidades arbitrárias
    vector<int> route = {0, 1, 0}; // Começa com a cidade 0 e 1 e retorna à 0
    vector<bool> visited(n, false);
    visited[0] = true;
    visited[1] = true;

    while (route.size() < n + 1) {
        double bestIncrease = numeric_limits<double>::infinity();
        int bestCity = -1;
        int bestPosition = -1;

        for (int city = 0; city < n; ++city) {
            if (visited[city]) continue;

            for (int pos = 0; pos < route.size() - 1; ++pos) {
                double increase = costMatrix[route[pos]][city] +
                                 costMatrix[city][route[pos + 1]] -
                                 costMatrix[route[pos]][route[pos + 1]];

                if (increase < bestIncrease) {
                    bestIncrease = increase;
                    bestCity = city;
                    bestPosition = pos;
                }
            }
        }

        // Insere a melhor cidade encontrada na melhor posição
        route.insert(route.begin() + bestPosition + 1, bestCity);
        visited[bestCity] = true;
    }

    // Calcula o custo total da rota final
    double bestCost = calculatePathCost(route, costMatrix);

    return {route, bestCost};
}

// Método de Reversão de Subcaminho (2-opt)
pair<vector<int>, double> twoOpt(const vector<int>& initialPath, const Matrix& costMatrix) {
    vector<int> bestPath = initialPath;
    double bestCost = calculatePathCost(bestPath, costMatrix);
    bool improved = true;

    while (improved) {
        improved = false;

        for (size_t i = 1; i < bestPath.size() - 2; ++i) {
            for (size_t j = i + 1; j < bestPath.size() - 1; ++j) {
                vector<int> newPath = bestPath;
                reverse(newPath.begin() + i, newPath.begin() + j + 1);

                double newCost = calculatePathCost(newPath, costMatrix);
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

// Função para salvar os resultados em um arquivo CSV
void saveResults(const string& outputFile, const string& mode, const vector<int>& route, double cost, const vector<string>& cities) {
    ofstream outFile(outputFile, ios::app);

    if (!outFile.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << outputFile << endl;
        return;
    }

    // Cabeçalho na primeira execução
    static bool headerWritten = false;
    if (!headerWritten) {
        outFile << "Modo,Rota,Custo\n";
        headerWritten = true;
    }

    outFile << mode << ",\"";
    for (int city : route) {
        outFile << cities[city] << " ";
    }
    outFile << "\"," << cost << "\n";

    outFile.close();
}

// Função principal
int main() {
    // Caminhos dos arquivos
    string distanceFile = "../Km_modificado.csv";
    string timeFile = "../Min_modificado.csv";
    string citiesFile = "../Cidades.csv";
    string outputFile = "../resultados_subcaminho_2.csv";

    // Carregar a matriz de distâncias
    cout << "Carregando a matriz de distâncias..." << endl;
    Matrix distanceMatrix = loadMatrixFromCSV(distanceFile);

    // Carregar a matriz de tempos
    cout << "Carregando a matriz de tempos..." << endl;
    Matrix timeMatrix = loadMatrixFromCSV(timeFile);

    // Carregar os nomes das cidades
    cout << "Carregando os nomes das cidades..." << endl;
    vector<string> cities = loadCitiesFromCSV(citiesFile);

    // Verificar se os dados foram carregados corretamente
    if (distanceMatrix.empty() || timeMatrix.empty() || cities.empty()) {
        cerr << "Erro: Dados não carregados corretamente." << endl;
        return 1;
    }

    // Aplicar o Algoritmo da Inserção Mais Barata com distâncias
    auto [initialRouteDist, initialCostDist] = insercaoMaisBarata(distanceMatrix);
    cout << "Custo inicial (Distância): " << initialCostDist << endl;
    saveResults(outputFile, "Distância - Inserção Mais Barata", initialRouteDist, initialCostDist, cities);

    // Aplicar o método 2-opt com distâncias
    auto [optimizedRouteDist, optimizedCostDist] = twoOpt(initialRouteDist, distanceMatrix);
    cout << "Custo otimizado (Distância - 2-opt): " << optimizedCostDist << endl;
    saveResults(outputFile, "Distância - 2-opt", optimizedRouteDist, optimizedCostDist, cities);

    // Aplicar o Algoritmo da Inserção Mais Barata com tempos
    auto [initialRouteTime, initialCostTime] = insercaoMaisBarata(timeMatrix);
    cout << "Custo inicial (Tempo): " << initialCostTime << endl;
    saveResults(outputFile, "Tempo - Inserção Mais Barata", initialRouteTime, initialCostTime, cities);

    // Aplicar o método 2-opt com tempos
    auto [optimizedRouteTime, optimizedCostTime] = twoOpt(initialRouteTime, timeMatrix);
    cout << "Custo otimizado (Tempo - 2-opt): " << optimizedCostTime << endl;
    saveResults(outputFile, "Tempo - 2-opt", optimizedRouteTime, optimizedCostTime, cities);

    return 0;
}
