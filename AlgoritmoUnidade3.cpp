#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <xlnt/xlnt.hpp>

using namespace std;

// Função para carregar a matriz de custos do arquivo Excel
vector<vector<double>> carregarMatrizExcel(const string& nomeArquivo, const string& aba) {
    vector<vector<double>> matriz;
    xlnt::workbook wb;

    try {
        wb.load(nomeArquivo); // Carregar o arquivo Excel

        if (find(wb.sheet_titles().begin(), wb.sheet_titles().end(), aba) == wb.sheet_titles().end()) {
            throw runtime_error("Aba não encontrada: " + aba);
        }

        xlnt::worksheet ws = wb.sheet_by_title(aba);

        for (auto row : ws.rows(false)) {
            vector<double> linha;
            for (auto cell : row) {
                if (cell.has_value()) {
                    linha.push_back(cell.value<double>());
                } else {
                    linha.push_back(0); // Preencher com zero se a célula estiver vazia
                }
            }
            matriz.push_back(linha);
        }
    } catch (const exception& e) {
        cerr << "Erro ao carregar o arquivo Excel: " << e.what() << endl;
    }

    return matriz;
}

// Calcula o custo total de uma rota
double calcularCusto(const vector<vector<double>>& matriz, const vector<int>& rota) {
    double custo = 0;
    for (int i = 0; i < rota.size() - 1; ++i) {
        custo += matriz[rota[i]][rota[i + 1]];
    }
    custo += matriz[rota.back()][rota.front()]; // Retorno à cidade inicial
    return custo;
}

// Algoritmo Guloso
double algoritmoGuloso(const vector<vector<double>>& matriz, int cidadeInicial, vector<int>& rota) {
    int n = matriz.size();
    vector<bool> visitado(n, false);
    rota.clear();
    rota.push_back(cidadeInicial);

    visitado[cidadeInicial] = true;
    double custoTotal = 0;
    int cidadeAtual = cidadeInicial;

    for (int i = 1; i < n; ++i) {
        double menorCusto = numeric_limits<double>::max();
        int proximaCidade = -1;

        for (int j = 0; j < n; ++j) {
            if (!visitado[j] && matriz[cidadeAtual][j] < menorCusto) {
                menorCusto = matriz[cidadeAtual][j];
                proximaCidade = j;
            }
        }

        custoTotal += menorCusto;
        rota.push_back(proximaCidade);
        visitado[proximaCidade] = true;
        cidadeAtual = proximaCidade;
    }

    custoTotal += matriz[cidadeAtual][cidadeInicial];
    rota.push_back(cidadeInicial);

    return custoTotal;
}

int main() {
    string nomeArquivo = "TCC_-_Matriz_do_problema.xlsx";
    string aba = "Km"; // Modifique para "Min" para usar a matriz de tempos

    vector<vector<double>> matriz = carregarMatrizExcel(nomeArquivo, aba);
    if (matriz.empty()) {
        cerr << "Erro: Matriz vazia ou não carregada corretamente." << endl;
        return 1;
    }

    int cidadeInicial = 0;
    cout << "Digite a cidade inicial (0 a " << matriz.size() - 1 << "): ";
    cin >> cidadeInicial;

    vector<int> rota;
    double custo = algoritmoGuloso(matriz, cidadeInicial, rota);

    cout << "Rota encontrada: ";
    for (int cidade : rota) {
        cout << cidade << " ";
    }
    cout << endl;
    cout << "Custo total: " << custo << endl;

    return 0;
}
