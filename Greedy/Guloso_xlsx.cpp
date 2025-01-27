#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <OpenXLSX.hpp> // Inclua a biblioteca OpenXLSX

using namespace OpenXLSX;
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

// Função para carregar a matriz de distâncias a partir de um arquivo .xlsx
vector<vector<double>> loadMatrixFromXLSX(const string& filePath, const string& sheetName) {
    vector<vector<double>> matrix;
    XLDocument doc;

    try {
        doc.open(filePath); // Abre o arquivo Excel
        auto worksheet = doc.workbook().worksheet(sheetName);

        size_t rowCount = worksheet.rowCount();
        size_t colCount = worksheet.columnCount();

        for (size_t row = 2; row <= rowCount; ++row) { // Ignorar cabeçalho
            vector<double> rowValues;
            for (size_t col = 2; col <= colCount; ++col) { // Ignorar a primeira coluna de índices
                auto cellValue = worksheet.cell(row, col).value();
                if (!cellValue.empty()) {
                    rowValues.push_back(cellValue.get<double>());
                } else {
                    rowValues.push_back(0.0); // Caso esteja vazio, adiciona 0
                }
            }
            matrix.push_back(rowValues);
        }

        doc.close();
    } catch (const exception& e) {
        cerr << "Erro ao carregar a matriz do arquivo Excel: " << e.what() << endl;
    }

    return matrix;
}

int main() {
    // Caminho do arquivo .xlsx e nome da planilha
    string filePath = "matriz.xlsx";
    string sheetName = "Km"; // Nome da aba que contém a matriz

    // Carregar a matriz de distâncias do arquivo Excel
    vector<vector<double>> distanceMatrix = loadMatrixFromXLSX(filePath, sheetName);

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
