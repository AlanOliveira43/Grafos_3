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

// Algoritmo de Inserção Mais Barata (City Insertion)
pair<vector<int>, double> insercaoMaisBarataCityInsertion(const Matrix& costMatrix) {
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

        // Avaliar todas as cidades não visitadas
        for (int city = 0; city < n; ++city) {
            if (visited[city]) continue;

            // Testar inserção da cidade em todas as posições possíveis no percurso
            for (int pos = 0; pos < route.size() - 1; ++pos) {
                double increase = costMatrix[route[pos]][city] +
                                 costMatrix[city][route[pos + 1]] -
                                 costMatrix[route[pos]][route[pos + 1]];

                // Atualizar melhor inserção encontrada
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

    // Passo 4: Calcular o custo total da rota final
    double bestCost = calculatePathCost(route, costMatrix);

    return {route, bestCost};
}

// Função principal para testar o algoritmo
int main() {
    // Caminho para o arquivo .csv com a matriz de custos
    string filePath = "../matriz_1_modificado.csv";

    // Carregar a matriz de custos
    Matrix costMatrix = loadMatrixFromCSV(filePath);

    if (costMatrix.empty()) {
        cerr << "Erro: Matriz de custos não carregada." << endl;
        return 1;
    }

    // Executar o algoritmo de Inserção Mais Barata
    auto result = insercaoMaisBarataCityInsertion(costMatrix);

    // Exibir os resultados
    cout << "Melhor rota encontrada: ";
    for (int city : result.first) {
        cout << city << " ";
    }
    cout << endl;

    cout << "Custo total da rota: " << result.second << endl;

    return 0;
}
