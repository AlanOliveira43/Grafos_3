#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

typedef vector<vector<double>> Matrix;

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

// Função para carregar os nomes das cidades
vector<string> loadCitiesFromCSV(const string& filePath) {
    vector<string> cities;
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return cities;
    }

    string line;
    while (getline(file, line)) {
        cities.push_back(line);
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
pair<vector<int>, double> insertionMaisBarata(const Matrix& costMatrix) {
    int n = costMatrix.size(); // Número de cidades

    vector<int> route = {0, 1, 0}; // Começar com a cidade 0 e 1 e voltar à 0
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

        route.insert(route.begin() + bestPosition + 1, bestCity);
        visited[bestCity] = true;
    }

    double bestCost = calculatePathCost(route, costMatrix);

    return {route, bestCost};
}

// Função para cortar a matriz e o vetor de cidades para o tamanho desejado
pair<Matrix, vector<string>> sliceMatrixAndCities(const Matrix& costMatrix, const vector<string>& cities, int size) {
    Matrix slicedMatrix(size, vector<double>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            slicedMatrix[i][j] = costMatrix[i][j];
        }
    }

    vector<string> slicedCities(cities.begin(), cities.begin() + size);
    return {slicedMatrix, slicedCities};
}

// Função para executar e registrar os resultados
void executeAndCompare(const Matrix& costMatrix, const vector<string>& cities, const string& outputFile, const string& mode, int problemNumber) {
    if (costMatrix.size() != cities.size()) {
        cerr << "Erro: O tamanho da matriz de custos não corresponde ao número de cidades." << endl;
        return;
    }

    ofstream outFile(outputFile, ios::app); // Abre em modo append
    if (!outFile.is_open()) {
        cerr << "Erro ao criar o arquivo de saída: " << outputFile << endl;
        return;
    }

    // Escrever cabeçalho apenas na primeira execução
    static bool headerWritten = false;
    if (!headerWritten) {
        outFile << "Problema,Número de Cidades,Rota,Custo,Tempo(ms),Modo\n";
        headerWritten = true;
    }

    auto start = high_resolution_clock::now();
    auto [route, cost] = insertionMaisBarata(costMatrix);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start).count();

    // Registrar resultados
    outFile << problemNumber << "," << cities.size() << ",\"";
    for (int city : route) {
        outFile << cities[city] << " ";
    }
    outFile << "\"," << cost << "," << duration << "," << mode << "\n";

    // Exibir no console
    cout << "Problema " << problemNumber << ": Rota encontrada: ";
    for (int city : route) {
        cout << cities[city] << " ";
    }
    cout << "| Custo: " << cost << " | Tempo: " << duration << " ms | Modo: " << mode << endl;

    outFile.close();
}

// Função principal
int main() {
    string distanceFile = "Km.csv";   // Caminho do arquivo CSV com distâncias
    string timeFile = "Min.csv";      // Caminho do arquivo CSV com tempos
    string citiesFile = "Cidades.csv"; // Caminho do arquivo CSV com nomes das cidades
    string outputFile = "resultados.csv"; // Arquivo de saída

    Matrix distanceMatrix = loadMatrixFromCSV(distanceFile);
    Matrix timeMatrix = loadMatrixFromCSV(timeFile);
    vector<string> cities = loadCitiesFromCSV(citiesFile);

    if (distanceMatrix.empty() || timeMatrix.empty() || cities.empty()) {
        cerr << "Erro: Dados não carregados corretamente." << endl;
        return 1;
    }

    // Tamanhos a serem testados
    vector<int> sizes = {48, 36, 24, 12, 7, 6};
    int problemNumber = 1;

    for (int size : sizes) {
        auto [slicedDistanceMatrix, slicedCities] = sliceMatrixAndCities(distanceMatrix, cities, size);
        auto [slicedTimeMatrix, slicedTimeCities] = sliceMatrixAndCities(timeMatrix, cities, size);

        // Executar para distâncias
        executeAndCompare(slicedDistanceMatrix, slicedCities, outputFile, "Distância", problemNumber);

        // Executar para tempos
        executeAndCompare(slicedTimeMatrix, slicedTimeCities, outputFile, "Tempo", problemNumber);

        problemNumber++;
    }

    return 0;
}
