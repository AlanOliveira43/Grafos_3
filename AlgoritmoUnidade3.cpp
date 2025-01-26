#include <iostream>
#include <vector>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <xlnt/xlnt.hpp> // Inclua a biblioteca xlnt

using namespace std;

// Funções comuns aos algoritmos

// Carrega a matriz de custos de um arquivo
vector<vector<double>> carregarMatriz(const string& nomeArquivo) {
    vector<vector<double>> matriz;
    ifstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return matriz;
    }

    string linha;
    while (getline(arquivo, linha)) {
        vector<double> linhaMatriz;
        stringstream ss(linha);
        double valor;
        while (ss >> valor) {
            linhaMatriz.push_back(valor);
        }
        matriz.push_back(linhaMatriz);
    }

    arquivo.close();
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

// Algoritmo da Inserção Mais Barata
double algoritmoInsercaoMaisBarata(const vector<vector<double>>& matriz, int cidadeInicial, vector<int>& rota) {
    int n = matriz.size();
    vector<bool> visitado(n, false);
    rota.clear();
    rota.push_back(cidadeInicial);
    rota.push_back(cidadeInicial); // Fechando o ciclo

    visitado[cidadeInicial] = true;
    double custoTotal = 0;

    for (int i = 1; i < n; ++i) {
        int melhorCidade = -1;
        double menorCusto = numeric_limits<double>::max();

        for (int j = 0; j < n; ++j) {
            if (!visitado[j]) {
                for (int k = 0; k < rota.size() - 1; ++k) {
                    double custoAtual = matriz[rota[k]][j] + matriz[j][rota[k + 1]] - matriz[rota[k]][rota[k + 1]];
                    if (custoAtual < menorCusto) {
                        menorCusto = custoAtual;
                        melhorCidade = j;
                    }
                }
            }
        }

        rota.insert(rota.end() - 1, melhorCidade);
        visitado[melhorCidade] = true;
        custoTotal += menorCusto;
    }

    return custoTotal;
}

// GRASP + Busca Local
vector<int> grasp(const vector<vector<double>>& matriz, int cidadeInicial, int maxIteracoes) {
    int n = matriz.size();
    vector<int> melhorRota;
    double melhorCusto = numeric_limits<double>::max();

    for (int iteracao = 0; iteracao < maxIteracoes; ++iteracao) {
        vector<int> rota;
        vector<bool> visitado(n, false);

        rota.push_back(cidadeInicial);
        visitado[cidadeInicial] = true;

        while (rota.size() < n) {
            vector<pair<int, double>> candidatos;
            int cidadeAtual = rota.back();

            for (int i = 0; i < n; ++i) {
                if (!visitado[i]) {
                    candidatos.emplace_back(i, matriz[cidadeAtual][i]);
                }
            }

            sort(candidatos.begin(), candidatos.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
                return a.second < b.second;
            });

            int limite = max(1, (int)(candidatos.size() * 0.2));
            int escolhido = rand() % limite;

            rota.push_back(candidatos[escolhido].first);
            visitado[candidatos[escolhido].first] = true;
        }

        double custoAtual = calcularCusto(matriz, rota);
        if (custoAtual < melhorCusto) {
            melhorCusto = custoAtual;
            melhorRota = rota;
        }
    }

    return melhorRota;
}

int main() {
    srand(time(0));

    string nomeArquivo;
    cout << "Digite o nome do arquivo da matriz (ex: Km.txt): ";
    cin >> nomeArquivo;

    vector<vector<double>> matriz = carregarMatriz(nomeArquivo);
    if (matriz.empty()) {
        cerr << "Erro ao carregar a matriz." << endl;
        return 1;
    }

    cout << "Escolha o algoritmo: \n1 - Guloso\n2 - Inserção Mais Barata\n3 - GRASP + Busca Local\n";
    int opcao;
    cin >> opcao;

    int cidadeInicial = 0;
    cout << "Digite a cidade inicial (0 a " << matriz.size() - 1 << "): ";
    cin >> cidadeInicial;

    vector<int> rota;
    double custo;

    if (opcao == 1) {
        custo = algoritmoGuloso(matriz, cidadeInicial, rota);
    } else if (opcao == 2) {
        custo = algoritmoInsercaoMaisBarata(matriz, cidadeInicial, rota);
    } else if (opcao == 3) {
        rota = grasp(matriz, cidadeInicial, 100);
        custo = calcularCusto(matriz, rota);
    } else {
        cout << "Opção inválida!" << endl;
        return 1;
    }

    cout << "Rota encontrada: ";
    for (int cidade : rota) {
        cout << cidade << " ";
    }
    cout << endl;
    cout << "Custo total: " << custo << endl;

    return 0;
}
