#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

typedef vector<vector<double>> Matrix;

// Função para carregar a matriz de custos a partir de um arquivo .csv
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

    // Passo 1: Começar com duas cidades arbitrárias
    vector<int> route = {0, 1, 0}; // Começar com a cidade 0 e 1 e voltar à 0
    vector<bool> visited(n, false);
    visited[0] = true;
    visited[1] = true;

    // Passo 2: Inserir cidades restantes iterativamente na posição de menor custo
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

    // Calcular o custo total da rota
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
                // Criar uma nova rota com subcaminho revertido
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

// Função principal
int main() {
    // Caminho para o arquivo .csv com a matriz de custos
    string filePath = "matriz_1_modificado.csv";

    // Carregar a matriz de custos
    Matrix costMatrix = loadMatrixFromCSV(filePath);

    if (costMatrix.empty()) {
        cerr << "Erro: Matriz de custos não carregada." << endl;
        return 1;
    }

    // Aplicar o Algoritmo da Inserção Mais Barata
    auto [initialRoute, initialCost] = insercaoMaisBarata(costMatrix);

    cout << "Rota inicial (Inserção Mais Barata): ";
    for (int city : initialRoute) {
        cout << city << " ";
    }
    cout << "\nCusto inicial: " << initialCost << endl;

    // Aplicar o método de Reversão de Subcaminho (2-opt)
    auto [optimizedRoute, optimizedCost] = twoOpt(initialRoute, costMatrix);

    cout << "Rota otimizada (2-opt): ";
    for (int city : optimizedRoute) {
        cout << city << " ";
    }
    cout << "\nCusto otimizado: " << optimizedCost << endl;

    return 0;
}
