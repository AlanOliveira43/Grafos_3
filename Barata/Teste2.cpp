#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <cctype>
using namespace std;

// Função para carregar a matriz de distâncias a partir de um arquivo CSV
vector<vector<double>> carregarMatriz(const string &nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    vector<vector<double>> matriz;
    string linha;

    if (!arquivo.is_open()) {
        cerr << "Erro: Não foi possível abrir o arquivo " << nomeArquivo << endl;
        exit(1);
    }

    while (getline(arquivo, linha)) {
        stringstream ss(linha);
        string valor;
        vector<double> linhaMatriz;

        while (getline(ss, valor, ',')) {
            // Substituir vírgulas por pontos, caso existam
            for (char &c : valor) {
                if (c == ',') c = '.';
            }
            // Verificar se o valor não está vazio e se é numérico
            if (!valor.empty() && (isdigit(valor[0]) || valor[0] == '-')) {
                try {
                    linhaMatriz.push_back(stod(valor)); // Converte string para double
                } catch (const invalid_argument &e) {
                    cerr << "Aviso: Valor inválido ignorado no arquivo: " << valor << endl;
                    linhaMatriz.push_back(0.0); // Valor padrão
                }
            } else {
                linhaMatriz.push_back(0.0); // Valor padrão para entradas inválidas
            }
        }
        matriz.push_back(linhaMatriz);
    }

    if (matriz.empty()) {
        cerr << "Erro: O arquivo " << nomeArquivo << " está vazio ou mal formatado." << endl;
        exit(1);
    }

    return matriz;
}

// Função para encontrar a próxima cidade mais próxima
int encontrarCidadeMaisProxima(int cidadeAtual, const vector<vector<double>> &distancias, const vector<bool> &visitado) {
    double menorDistancia = numeric_limits<double>::max(); // Inicializa com valor máximo
    int proximaCidade = -1;

    for (int i = 0; i < distancias.size(); i++) {
        if (!visitado[i] && distancias[cidadeAtual][i] < menorDistancia) {
            menorDistancia = distancias[cidadeAtual][i];
            proximaCidade = i;
        }
    }
    return proximaCidade;
}

// Função principal do algoritmo guloso
pair<vector<int>, double> algoritmoGuloso(const vector<vector<double>> &distancias, int cidadeInicial) {
    int n = distancias.size();
    vector<bool> visitado(n, false); // Vetor para marcar cidades visitadas
    vector<int> rota;               // Vetor para armazenar a rota
    double custoTotal = 0.0;        // Custo total da rota

    int cidadeAtual = cidadeInicial;
    rota.push_back(cidadeAtual);
    visitado[cidadeAtual] = true;

    // Itera até visitar todas as cidades
    for (int i = 1; i < n; i++) {
        int proximaCidade = encontrarCidadeMaisProxima(cidadeAtual, distancias, visitado);
        if (proximaCidade == -1) {
            cerr << "Erro: Não foi possível encontrar uma cidade válida." << endl;
            exit(1);
        }
        custoTotal += distancias[cidadeAtual][proximaCidade];
        cidadeAtual = proximaCidade;
        rota.push_back(cidadeAtual);
        visitado[cidadeAtual] = true;
    }

    // Retorna à cidade inicial
    custoTotal += distancias[cidadeAtual][cidadeInicial];
    rota.push_back(cidadeInicial);

    return make_pair(rota, custoTotal);
}

// Função para salvar os resultados em um arquivo
void salvarResultados(const string &nomeArquivo, const pair<vector<int>, double> &resultado, const string &unidade) {
    ofstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro: Não foi possível criar o arquivo " << nomeArquivo << endl;
        exit(1);
    }

    arquivo << "Rota encontrada:" << endl;
    for (int cidade : resultado.first) {
        arquivo << cidade + 1 << " "; // Ajuste para exibir cidades com índice iniciando em 1
    }
    arquivo << endl;
    arquivo << "Custo total: " << resultado.second << " " << unidade << endl;

    arquivo.close();
    cout << "Resultados salvos em " << nomeArquivo << endl;
}

int main() {
    // Carregar os dados das distâncias automaticamente (Km_modificado.csv e Min_modificado.csv)
    string arquivoKm = "Km_modificado.csv";
    string arquivoMin = "Min_modificado.csv";

    cout << "Processando arquivo de distâncias em Km..." << endl;
    vector<vector<double>> distanciasKm = carregarMatriz(arquivoKm);
    cout << "Matriz de distâncias em Km carregada com sucesso. Dimensão: "
         << distanciasKm.size() << "x" << (distanciasKm.empty() ? 0 : distanciasKm[0].size()) << endl;

    cout << "Processando arquivo de distâncias em Minutos..." << endl;
    vector<vector<double>> distanciasMin = carregarMatriz(arquivoMin);
    cout << "Matriz de distâncias em Minutos carregada com sucesso. Dimensão: "
         << distanciasMin.size() << "x" << (distanciasMin.empty() ? 0 : distanciasMin[0].size()) << endl;

    // Definir a cidade inicial
    int cidadeInicial = 0; // Começa na primeira cidade (índice 0)

    // Executar o algoritmo guloso para Km
    cout << "\nResultados para distâncias em Km:" << endl;
    pair<vector<int>, double> resultadoKm = algoritmoGuloso(distanciasKm, cidadeInicial);
    salvarResultados("resultado_km.txt", resultadoKm, "Km");

    // Executar o algoritmo guloso para Min
    cout << "\nResultados para distâncias em Minutos:" << endl;
    pair<vector<int>, double> resultadoMin = algoritmoGuloso(distanciasMin, cidadeInicial);
    salvarResultados("resultado_minutos.txt", resultadoMin, "Minutos");

    return 0;
}
