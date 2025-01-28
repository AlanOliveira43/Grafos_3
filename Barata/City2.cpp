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

// Algoritmo da Inserção Mais Barata (City Insertion)
pair<vector<int>, double> insercaoMaisBarataCityInsertion(const Matrix& costMatrix) {
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

        // Inserir a melhor cidade encontrada na melhor posição
        route.insert(route.begin() + bestPosition + 1, bestCity);
        visited[bestCity] = true;
    }

    double bestCost = calculatePathCost(route, costMatrix);

    return {route, bestCost};
}

// Função para salvar os resultados em um arquivo CSV
void saveResults(const string& outputFile, const string& mode, const vector<int>& route, double cost, const vector<string>& cities) {
    ofstream outFile(outputFile, ios::app);

    if (!outFile.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << outputFile << endl;
        return;
    }

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
    string outputFile = "../resultados_city2.csv";

    // Carregar a matriz de distâncias
    cout << "Carregando a matriz de distâncias..." << endl;
    Matrix distanceMatrix = loadMatrixFromCSV(distanceFile);

    // Carregar a matriz de tempos
    cout << "Carregando a matriz de tempos..." << endl;
    Matrix timeMatrix = loadMatrixFromCSV(timeFile);

    // Carregar os nomes das cidades
    cout << "Carregando os nomes das cidades..." << endl;
    vector<string> cities = loadCitiesFromCSV(citiesFile);

    if (distanceMatrix.empty() || timeMatrix.empty() || cities.empty()) {
        cerr << "Erro: Dados não carregados corretamente." << endl;
        return 1;
    }

    // Aplicar o Algoritmo da Inserção Mais Barata (City Insertion) com distâncias
    auto [initialRouteDist, initialCostDist] = insercaoMaisBarataCityInsertion(distanceMatrix);
    cout << "Custo inicial (Distância - Inserção Mais Barata): " << initialCostDist << endl;
    saveResults(outputFile, "Distância - Inserção Mais Barata", initialRouteDist, initialCostDist, cities);

    // Aplicar o Algoritmo da Inserção Mais Barata (City Insertion) com tempos
    auto [initialRouteTime, initialCostTime] = insercaoMaisBarataCityInsertion(timeMatrix);
    cout << "Custo inicial (Tempo - Inserção Mais Barata): " << initialCostTime << endl;
    saveResults(outputFile, "Tempo - Inserção Mais Barata", initialRouteTime, initialCostTime, cities);

    return 0;
}
