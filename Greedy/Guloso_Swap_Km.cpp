#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

// Função para calcular o custo total de um percurso
double calculatePathCost(const vector<int>& path, const vector<vector<double>>& distanceMatrix) {
    double cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += distanceMatrix[path[i]][path[i + 1]];
    }
    return cost;
}

// Método de Troca de Vizinhos (Swap)
pair<vector<int>, double> swapNeighbors(const vector<int>& initialPath, const vector<vector<double>>& distanceMatrix) {
    vector<int> bestPath = initialPath;
    double bestCost = calculatePathCost(bestPath, distanceMatrix);
    bool improved = true;

    while (improved) {
        improved = false;
        for (size_t i = 1; i < bestPath.size() - 2; ++i) {
            for (size_t j = i + 1; j < bestPath.size() - 1; ++j) {
                vector<int> newPath = bestPath;
                swap(newPath[i], newPath[j]); // Troca as cidades i e j
                double newCost = calculatePathCost(newPath, distanceMatrix);

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

// Função para carregar a matriz de distâncias a partir de um arquivo CSV
vector<vector<double>> loadMatrixFromCSV(const string& filePath) {
    vector<vector<double>> matrix;
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filePath << endl;
        return matrix;
    }

    string line;
    bool isHeader = true; // Ignora o cabeçalho na primeira linha
    while (getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue; // Pula o cabeçalho
        }

        vector<double> row;
        stringstream ss(line);
        string value;

        while (getline(ss, value, ';')) { // Usa ponto e vírgula como separador
            replace(value.begin(), value.end(), ',', '.'); // Substitui vírgulas por pontos
            try {
                row.push_back(stod(value)); // Converte para double
            } catch (const invalid_argument&) {
                row.push_back(0); // Caso não seja possível converter, adiciona 0
            }
        }

        matrix.push_back(row);
    }

    file.close();
    return matrix;
}

int main() {
    // Caminho do arquivo .csv com a matriz de distâncias
    string filePath = "matriz.csv";

    // Carregar a matriz de distâncias
    vector<vector<double>> distanceMatrix = loadMatrixFromCSV(filePath);

    if (distanceMatrix.empty()) {
        cerr << "Erro: Matriz de distâncias não carregada." << endl;
        return 1;
    }

    // Inicializar o percurso (0 -> 1 -> 2 -> ... -> n-1 -> 0)
    size_t numCities = distanceMatrix.size();
    vector<int> initialPath(numCities + 1);
    for (size_t i = 0; i < numCities; ++i) {
        initialPath[i] = i;
    }
    initialPath[numCities] = 0; // Retorna à cidade inicial

    // Cálculo do custo inicial
    double initialCost = calculatePathCost(initialPath, distanceMatrix);

    // Aplicar o método de Troca de Vizinhos
    auto [optimizedPath, optimizedCost] = swapNeighbors(initialPath, distanceMatrix);

    // Exibir resultados
    cout << "Percurso inicial: ";
    for (int city : initialPath) {
        cout << city << " ";
    }
    cout << "\nCusto inicial: " << initialCost << endl;

    cout << "Percurso otimizado: ";
    for (int city : optimizedPath) {
        cout << city << " ";
    }
    cout << "\nCusto otimizado: " << optimizedCost << endl;

    return 0;
}
