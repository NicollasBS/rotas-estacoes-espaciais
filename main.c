#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_VERTICES 100
#define INF INT_MAX

typedef struct Aresta {
    int destino;
    int peso;
    struct Aresta* proxima;
} Aresta;

typedef struct {
    Aresta* cabeca;
} ListaAdjacente;

typedef struct {
    ListaAdjacente vertices[MAX_VERTICES];
    int numVertices;
    char nomesEstacoes[MAX_VERTICES][50];
} Grafo;

Grafo* criarGrafo(int numVertices) {
    Grafo* grafo = (Grafo*)malloc(sizeof(Grafo));
    grafo->numVertices = 0;
    for (int i = 0; i < numVertices; i++) {
        grafo->vertices[i].cabeca = NULL;
        strcpy(grafo->nomesEstacoes[i], "");
    }
    return grafo;
}


void adicionarAresta(Grafo* grafo, int origem, int destino, int peso) {
    if (origem < 0 || destino < 0 || origem >= MAX_VERTICES || destino >= MAX_VERTICES) return;

    Aresta* atual = grafo->vertices[origem].cabeca;
    while (atual != NULL) {
        if (atual->destino == destino) {
            return; 
        }
        atual = atual->proxima;
    }

    Aresta* novaAresta = (Aresta*)malloc(sizeof(Aresta));
    novaAresta->destino = destino;
    novaAresta->peso = peso;
    novaAresta->proxima = grafo->vertices[origem].cabeca;
    grafo->vertices[origem].cabeca = novaAresta;

    atual = grafo->vertices[destino].cabeca;
    while (atual != NULL) {
        if (atual->destino == origem) {
            return; 
        }
        atual = atual->proxima;
    }

    
    novaAresta = (Aresta*)malloc(sizeof(Aresta));
    novaAresta->destino = origem;
    novaAresta->peso = peso;
    novaAresta->proxima = grafo->vertices[destino].cabeca;
    grafo->vertices[destino].cabeca = novaAresta;
}


void imprimirCaminho(Grafo* grafo, int predecessores[], int vertice) {
    if (predecessores[vertice] == -1) {
        printf("%s", grafo->nomesEstacoes[vertice]);
        return;
    }
    imprimirCaminho(grafo, predecessores, predecessores[vertice]);
    printf(" -> %s", grafo->nomesEstacoes[vertice]);
}

int dijkstra(Grafo* grafo, int inicio, int fim, int predecessores[]) {
    int distancias[MAX_VERTICES], visitados[MAX_VERTICES] = {0};

    for (int i = 0; i < grafo->numVertices; i++) {
        distancias[i] = INF;
        predecessores[i] = -1;
    }
    distancias[inicio] = 0;

    for (int i = 0; i < grafo->numVertices - 1; i++) {
        int u = -1;
        for (int j = 0; j < grafo->numVertices; j++) {
            if (!visitados[j] && (u == -1 || distancias[j] < distancias[u])) u = j;
        }

        if (u == -1 || distancias[u] == INF) break;
        visitados[u] = 1;

        Aresta* atual = grafo->vertices[u].cabeca;
        while (atual != NULL) {
            int v = atual->destino;
            int peso = atual->peso;
            if (!visitados[v] && distancias[u] + peso < distancias[v]) {
                distancias[v] = distancias[u] + peso;
                predecessores[v] = u;
            }
            atual = atual->proxima;
        }
    }

    return distancias[fim];
}

int obterIndiceEstacao(Grafo* grafo, const char* nome) {
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->nomesEstacoes[i], nome) == 0) {
            return i;
        }
    }
    return -1;
}

void carregarGrafoDeCSV(Grafo* grafo, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo CSV.\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char nomeOrigem[50];
        int indiceOrigem, indiceDestino, peso;

        char* token = strtok(linha, ",");
        strcpy(nomeOrigem, token);

        indiceOrigem = obterIndiceEstacao(grafo, nomeOrigem);
        if (indiceOrigem == -1 && grafo->numVertices < MAX_VERTICES) {
            indiceOrigem = grafo->numVertices;
            strcpy(grafo->nomesEstacoes[grafo->numVertices++], nomeOrigem);
        }

        while ((token = strtok(NULL, ",")) != NULL) {
            char nomeDestino[50];
            strcpy(nomeDestino, token);

            token = strtok(NULL, ",");
            int pesoBruto = atoi(token);

            if (pesoBruto == 1) {
                peso = 1;
            } else if (pesoBruto == 6) {
                peso = 6;
            } else if (pesoBruto == 3) {
                peso = 3;
            } else {
                continue;
            }

            indiceDestino = obterIndiceEstacao(grafo, nomeDestino);
            if (indiceDestino == -1 && grafo->numVertices < MAX_VERTICES) {
                indiceDestino = grafo->numVertices;
                strcpy(grafo->nomesEstacoes[grafo->numVertices++], nomeDestino);
            }

            if (indiceOrigem != -1 && indiceDestino != -1) {
                adicionarAresta(grafo, indiceOrigem, indiceDestino, peso);
            }
        }
    }
    fclose(arquivo);
}

void removerArestasCaminho(Grafo* grafo, int predecessores[], int inicio, int fim) {
    int atual = fim;
    while (atual != inicio) {
        int anterior = predecessores[atual];
        Aresta** aresta = &grafo->vertices[anterior].cabeca;
        while (*aresta != NULL) {
            if ((*aresta)->destino == atual) {
                Aresta* temp = *aresta;
                *aresta = (*aresta)->proxima;
                free(temp);
                break;
            }
            aresta = &(*aresta)->proxima;
        }
        atual = anterior;
    }
}

void restaurarArestas(Grafo* grafo, int predecessores[], int inicio, int fim, int peso) {
    int atual = fim;
    while (atual != inicio) {
        int anterior = predecessores[atual];
        adicionarAresta(grafo, anterior, atual, peso);
        atual = anterior;
    }
}

void encontrarCaminhosAlternativos(Grafo* grafo, int inicio, int fim) {
    int predecessores[MAX_VERTICES];
    int caminhoPrincipal = dijkstra(grafo, inicio, fim, predecessores);

    if (caminhoPrincipal != INF) {
        printf("\n\033[0;34mCaminho principal encontrado:\033[0m\n");
        printf("\t");
        imprimirCaminho(grafo, predecessores, fim);
        printf("\n\tCusto total: %d\n", caminhoPrincipal);

        removerArestasCaminho(grafo, predecessores, inicio, fim);

        for(int i = 0; i < 10; i++){
            int caminhoAlternativo = dijkstra(grafo, inicio, fim, predecessores);
            if (caminhoAlternativo != INF) {
                printf("\n\n\033[0;33mCaminho alternativo encontrado:\033[0m\n");
                printf("\t");
                imprimirCaminho(grafo, predecessores, fim);
                printf("\n\tCusto total: %d\n", caminhoAlternativo);

                removerArestasCaminho(grafo, predecessores, inicio, fim);

            }
        }
    } else {
        printf("Não foi possível encontrar um caminho entre as estações %s e %s.\n", grafo->nomesEstacoes[inicio], grafo->nomesEstacoes[fim]);
    }

    
    carregarGrafoDeCSV(grafo, "rotas.csv");
}


void imprimirGrafo(Grafo* grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        printf("Estação \033[1;31m%s\033[0m está conectada a:\n", grafo->nomesEstacoes[i]);
        Aresta* aresta = grafo->vertices[i].cabeca;
        while (aresta != NULL) {
            printf("  - %s (Peso: %d)\n", grafo->nomesEstacoes[aresta->destino], aresta->peso);
            aresta = aresta->proxima;
        }
        printf("\n");
    }
}

void imprimirConexoes(Grafo* grafo, const char* nome){
    for (int i = 0; i < grafo->numVertices; i++) {
        if(strcmp(grafo->nomesEstacoes[i], nome) == 0){
            printf("Estação %s está conectada a:\n", nome);
            Aresta* aresta = grafo->vertices[i].cabeca;
            while (aresta != NULL) {
                printf("  - %s (Peso: %d)\n", grafo->nomesEstacoes[aresta->destino], aresta->peso);
                aresta = aresta->proxima;
            }
            return;
        }
    }
    printf("Estação não encontrada.\n");
}




void dfs(Grafo* grafo, int vertice, int visitados[]) {
    visitados[vertice] = 1;
    Aresta* aresta = grafo->vertices[vertice].cabeca;
    while (aresta != NULL) {
        if (!visitados[aresta->destino]) {
            dfs(grafo, aresta->destino, visitados);
        }
        aresta = aresta->proxima;
    }
}

void dfsInverso(Grafo* grafo, int vertice, int visitados[]) {
    visitados[vertice] = 1;
    Aresta* aresta = grafo->vertices[vertice].cabeca;
    while (aresta != NULL) {
        if (!visitados[aresta->destino]) {
            dfsInverso(grafo, aresta->destino, visitados);
        }
        aresta = aresta->proxima;
    }
}

int verificarConectividade(Grafo* grafo) {
    int visitados[MAX_VERTICES] = {0};

    
    dfs(grafo, 0, visitados);

    for (int i = 0; i < grafo->numVertices; i++) {
        if (visitados[i] == 0) {
            return 0; 
        }
    }

    Grafo* grafoInvertido = criarGrafo(MAX_VERTICES);
    for (int i = 0; i < grafo->numVertices; i++) {
        Aresta* aresta = grafo->vertices[i].cabeca;
        while (aresta != NULL) {
            adicionarAresta(grafoInvertido, aresta->destino, i, aresta->peso);
            aresta = aresta->proxima;
        }
    }

    memset(visitados, 0, sizeof(visitados)); 
    dfsInverso(grafoInvertido, 0, visitados);

    for (int i = 0; i < grafo->numVertices; i++) {
        if (visitados[i] == 0) {
            return 0; 
        }
    }

    return 1; 
}

void removerEstacao(Grafo* grafo, const char* nomeEstacao) {
    int indiceEstacao = obterIndiceEstacao(grafo, nomeEstacao);
    
    if (indiceEstacao == -1) {
        printf("Estação não encontrada.\n");
        return;
    }

    
    for (int i = 0; i < grafo->numVertices; i++) {
        Aresta** aresta = &grafo->vertices[i].cabeca;
        while (*aresta != NULL) {
            if ((*aresta)->destino == indiceEstacao) {
                Aresta* temp = *aresta;
                *aresta = (*aresta)->proxima;
                free(temp);
            } else {
                aresta = &(*aresta)->proxima;
            }
        }
    }

    Aresta* aresta = grafo->vertices[indiceEstacao].cabeca;
    while (aresta != NULL) {
        Aresta* temp = aresta;
        aresta = aresta->proxima;
        free(temp); 
    }
    grafo->vertices[indiceEstacao].cabeca = NULL;

    
    for (int i = indiceEstacao; i < grafo->numVertices - 1; i++) {
        
        strcpy(grafo->nomesEstacoes[i], grafo->nomesEstacoes[i + 1]);
        grafo->vertices[i] = grafo->vertices[i + 1];
    }

    grafo->numVertices--;
    printf("Estação %s removida com sucesso.\n", nomeEstacao);
}




void mostrarMenu(){
    printf("\n------------------------------------------------------");
    printf("\n| [1]Encontrar caminho entre duas estações");
    printf("\n| [2]Mostrar todos os caminhos");
    printf("\n| [3]Mostrar o conexões de uma estação");
    printf("\n| [4]Verificar se as estações são fortemente conectadas");
    printf("\n| [5]Excluir manualmente uma estação");
    printf("\n|");
    printf("\n| [0]Sair");
}

int main() {
    Grafo* grafo = criarGrafo(MAX_VERTICES);
    carregarGrafoDeCSV(grafo, "rotas.csv");

    clock_t inicio, fim;
    double tempoExecucao;

    int opc = -1;
    while(opc != 0){
        mostrarMenu();
        printf("\n|\n| Opção: ");
        scanf("%d", &opc);
        switch (opc){
            case 1:
                printf("\n\033[1;34m----------------CAMINHO ENTRE ESTAÇÕES----------------\033[0m\n");
                char estacaoInicio[50], estacaoFim[50];
                printf("Digite o nome da estação de partida: ");
                scanf("%s", estacaoInicio);
                printf("Digite o nome da estação de destino: ");
                scanf("%s", estacaoFim);
                inicio = clock();

                int inicio = obterIndiceEstacao(grafo, estacaoInicio);
                int fim = obterIndiceEstacao(grafo, estacaoFim);

                if (inicio == -1 || fim == -1) {
                    printf("Uma ou ambas as estações fornecidas não existem no grafo.\n");
                } else {
                    encontrarCaminhosAlternativos(grafo, inicio, fim);
                }
                printf("\033[1;34m------------------------------------------------------\033[0m\n");
                fim = clock();
                tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                printf("Tempo de execução: %f segundos\n", tempoExecucao);
                break;

            case 2:
                printf("\033[1;33m------------------TODAS OS CAMINHOS------------------\033[0m\n");
                inicio = clock();
                imprimirGrafo(grafo);
                printf("\033[1;33m-----------------------------------------------------\033[0m\n");fim = clock();
                tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                printf("Tempo de execução: %f segundos\n", tempoExecucao);
                break;

            case 3:
                printf("\033[1;32m---------------CONEXÕES DE UMA ESTAÇÃO---------------\033[0m\n");
                char estacaoProc[50];
                printf("Digite o nome da estação para mostrar as conexões: ");
                scanf("%s", estacaoProc);
                printf("\n");
                inicio = clock();
                imprimirConexoes(grafo, estacaoProc);
                printf("\033[1;32m-----------------------------------------------------\033[0m\n");
                fim = clock();
                tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                printf("Tempo de execução: %f segundos\n", tempoExecucao);
                break;

            case 4:
                inicio = clock();
                if (verificarConectividade(grafo)) {
                    printf("\033[1;32mO grafo é fortemente conectado.\033[0m\n");
                } else {
                    printf("\033[1;31mO grafo NÃO é fortemente conectado.\033[0m\n");
                }
                fim = clock();
                tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                printf("Tempo de execução: %f segundos\n", tempoExecucao);
                break;      

            case 5:          
                printf("\033[1;31m-------------EXCLUSÃO MANUAL DE ESTAÇÃO--------------\033[0m\n");                
                char estacaoExc[50];
                printf("Digite o nome da estação para excluir do mapa: ");
                scanf("%s", estacaoExc);  
                char ctz[5];
                printf("\033[30;41mTenha certeza que deseja excluir esta estação? [S]sim/[N]não\033[0m\n ");
                scanf("%s", ctz);
                if (strcmp(ctz, "S") == 0){
                    inicio = clock();
                    removerEstacao(grafo, estacaoExc);
                    printf("\033[1;31m-----------------------------------------------------\033[0m\n");
                    fim = clock();
                    tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                    printf("Tempo de execução: %f segundos\n", tempoExecucao);
                    break;
                }
                printf("\033[1;31mOperação Cancelada\033[0m\n");
                printf("\033[1;31m-----------------------------------------------------\033[0m\n");
                fim = clock();
                tempoExecucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                printf("Tempo de execução: %f segundos\n", tempoExecucao);
                break;

            case 0:
                printf("\n\n\nObrigado.\n\n\n");
                opc = 0;
                break;
        }
    }


    

    return 0;
}
