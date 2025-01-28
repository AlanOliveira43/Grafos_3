#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <limits>
#include <chrono> // Incluindo chrono para medir o tempo

using namespace std;
using namespace chrono; // Para facilitar o uso das funções de medição de tempo

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

// Método de Troca de Vizinhos (Swap)
pair<vector<int>, double> swapNeighbors(const vector<int>& initialPath, const Matrix& costMatrix) {
    vector<int> bestPath = initialPath;
    double bestCost = calculatePathCost(bestPath, costMatrix);
    bool improved = true;

    while (improved) {
        improved = false;
        for (size_t i = 1; i < bestPath.size() - 2; ++i) {
            for (size_t j = i + 1; j < bestPath.size() - 1; ++j) {
                vector<int> newPath = bestPath;
                swap(newPath[i], newPath[j]); // Troca as cidades i e j
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
void saveResults(const string& outputFile, const string& mode, const vector<int>& route, double cost, const vector<string>& cities, double executionTime) {
    ofstream outFile(outputFile, ios::app);

    if (!outFile.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << outputFile << endl;
        return;
    }

    static bool headerWritten = false;
    if (!headerWritten) {
        outFile << "Modo,Rota,Custo,Tempo (s)\n";
        headerWritten = true;
    }

    outFile << mode << ",\"";
    for (int city : route) {
        outFile << cities[city] << " ";
    }
    outFile << "\"," << cost << "," << executionTime << "\n";

    outFile.close();
}

// Função principal
int main() {
    // Caminhos dos arquivos
    string distanceFile = "../Km_modificado.csv";
    string timeFile = "../Min_modificado.csv";
    string citiesFile = "../Cidades.csv";
    string outputFile = "../resultados_g2.csv";

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

    // Inicializar o percurso (0 -> 1 -> 2 -> ... -> n-1 -> 0)
    size_t numCities = distanceMatrix.size();
    vector<int> initialPath(numCities + 1);
    for (size_t i = 0; i < numCities; ++i) {
        initialPath[i] = i;
    }
    initialPath[numCities] = 0; // Retorna à cidade inicial

    // Medir tempo para o custo inicial e otimização por distância
    auto start = high_resolution_clock::now();
    auto [optimizedPathDist, optimizedCostDist] = swapNeighbors(initialPath, distanceMatrix);
    auto end = high_resolution_clock::now();
    double executionTimeDist = duration_cast<duration<double>>(end - start).count();

    cout << "Custo otimizado (Distância - Troca de Vizinhos): " << optimizedCostDist << " | Tempo: " << executionTimeDist << "s" << endl;
    saveResults(outputFile, "Distância - Troca de Vizinhos", optimizedPathDist, optimizedCostDist, cities, executionTimeDist);

    // Medir tempo para o custo inicial e otimização por tempo
    start = high_resolution_clock::now();
    auto [optimizedPathTime, optimizedCostTime] = swapNeighbors(initialPath, timeMatrix);
    end = high_resolution_clock::now();
    double executionTimeTime = duration_cast<duration<double>>(end - start).count();

    cout << "Custo otimizado (Tempo - Troca de Vizinhos): " << optimizedCostTime << " | Tempo: " << executionTimeTime << "s" << endl;
    saveResults(outputFile, "Tempo - Troca de Vizinhos", optimizedPathTime, optimizedCostTime, cities, executionTimeTime);

    return 0;
}
