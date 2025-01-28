#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

typedef vector<vector<double>> Matrix;

// Função para carregar a matriz de custos de um arquivo CSV
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

// Função de busca local (2-opt)
void localSearch2Opt(vector<int>& route, const Matrix& costMatrix) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 2; ++i) {
            for (size_t j = i + 1; j < route.size() - 1; ++j) {
                // Cria uma nova rota invertendo o subcaminho entre i e j
                vector<int> newRoute = route;
                reverse(newRoute.begin() + i, newRoute.begin() + j + 1);

                // Calcula o custo da rota atual e da nova rota
                double oldCost = calculateRouteCost(route, costMatrix);
                double newCost = calculateRouteCost(newRoute, costMatrix);

                // Se a nova rota for melhor, aceita a modificação
                if (newCost < oldCost) {
                    route = newRoute;
                    improved = true;
                }
            }
        }
    }
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
    string outputFile = "../resultados_sub_greedy.csv";

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

    // Inicializar a rota (0 → 1 → 2 → ... → n-1 → 0)
    vector<int> route;
    for (size_t i = 0; i < distanceMatrix.size(); ++i) {
        route.push_back(i);
    }
    route.push_back(route[0]); // Retorna à cidade inicial

    // Cálculo do custo inicial para distância
    double initialCostDist = calculateRouteCost(route, distanceMatrix);

    // Aplicar a busca local 2-opt para otimizar a distância
    localSearch2Opt(route, distanceMatrix);
    double optimizedCostDist = calculateRouteCost(route, distanceMatrix);
    cout << "Custo otimizado (Distância - 2-opt): " << optimizedCostDist << endl;
    saveResults(outputFile, "Distância - 2-opt", route, optimizedCostDist, cities);

    // Resetar a rota inicial
    route.clear();
    for (size_t i = 0; i < timeMatrix.size(); ++i) {
        route.push_back(i);
    }
    route.push_back(route[0]); // Retorna à cidade inicial

    // Cálculo do custo inicial para tempo
    double initialCostTime = calculateRouteCost(route, timeMatrix);

    // Aplicar a busca local 2-opt para otimizar o tempo
    localSearch2Opt(route, timeMatrix);
    double optimizedCostTime = calculateRouteCost(route, timeMatrix);
    cout << "Custo otimizado (Tempo - 2-opt): " << optimizedCostTime << endl;
    saveResults(outputFile, "Tempo - 2-opt", route, optimizedCostTime, cities);

    return 0;
}
