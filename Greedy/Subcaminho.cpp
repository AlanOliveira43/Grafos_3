#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

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

// Função principal para testar o algoritmo 2-opt
int main() {
    // Caminho para o arquivo CSV contendo a matriz de custos
    string filePath = "../matriz_1_modificado.csv";

    // Carregar a matriz de custos do arquivo CSV
    Matrix costMatrix = loadMatrixFromCSV(filePath);

    if (costMatrix.empty()) {
        cerr << "Erro: Matriz de custos não carregada." << endl;
        return 1;
    }

    // Rota inicial (exemplo: 0 → 1 → 2 → 3 → ... → n-1 → 0)
    vector<int> route;
    for (size_t i = 0; i < costMatrix.size(); ++i) {
        route.push_back(i);
    }

    // Calcula o custo inicial da rota
    double initialCost = calculateRouteCost(route, costMatrix);
    cout << "Custo inicial da rota: " << initialCost << endl;

    // Aplica a busca local 2-opt
    localSearch2Opt(route, costMatrix);

    // Calcula o custo final da rota
    double finalCost = calculateRouteCost(route, costMatrix);
    cout << "Custo final da rota após 2-opt: " << finalCost << endl;

    // Exibe a rota final
    cout << "Rota final: ";
    for (int city : route) {
        cout << city << " ";
    }
    cout << route[0] << endl; // Volta ao ponto inicial

    return 0;
}