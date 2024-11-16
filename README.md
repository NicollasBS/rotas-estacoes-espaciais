# 🛸 Sistema de Rotas Espaciais - Teoria de Grafos

Este projeto implementa um sistema de rotas utilizando **Grafos** e o **Algoritmo de Dijkstra** para encontrar o menor caminho e caminhos alternativos entre estações espaciais. 

---

## 📋 Funcionalidades

- **Carregar Grafo de um CSV**  
  Lê um arquivo CSV para criar as conexões (arestas) entre as estações (vértices).

- **Adicionar Arestas e Vértices a partir de um documento CSV**  
  Permite adicionar novas conexões com pesos personalizados.

- **Encontrar Caminhos**  
  Utiliza o Algoritmo de Dijkstra para:
  - Encontrar o menor caminho entre duas estações quaisquer.
  - Descobrir caminhos alternativos caso o principal esteja indisponível.

- **Mostrar caminhos**
  Possibilidade de listar todas as conexões do arquivo CSV e também das alterações.
  - Além de listar todas as conexões, há a possibilidade de procurar uma estação específica para ver suas conexões.

- **Fortemente conecado ou não?**
  Verificar se o grafo carregado a partir do arquivo CSV é fortemente conectado ou não.

- **Excluir manualmente uma estação**
  É possível recalcular a rota até determinada estação de forma forçada.
  - Para isso é necessário realizar a exclusão manual de uma estação.
  - Só é possível restaurar a estacão excluída como o recarregamento dos caminhos do arquivo CSV.
  - *Só realize a exclusão por real necessidade.*

---

## 🛠️ Estrutura do Projeto

### Arquivos principais:
- `main.c`  
  Contém a lógica principal do programa.

- `rotas.csv`  
  Arquivo com as conexões entre estações no formato:  
  **EstaçãoOrigem,EstaçãoDestino,Peso**

### Exemplo de `rotas.csv`:
```csv
EstaçãoA,EstaçãoB,1
EstaçãoB,EstaçãoC,3
EstaçãoC,EstaçãoD,6
```
---

## 📂 Estrutura de Dados
  O projeto utiliza **listas de adjacência** para representar o grafo. Cada vértice contém uma lista de arestas que indicam as conexões com outros vértices.

### Estruturas principais:
- `Grafo`
  - Número de vértices.
  - Listas de adjacência.
  - Nomes das estações.

- `Arestas`- representa uma conexão entre dois vértices:
  - Peso
  - Destino

---

## 🚀 Como Executar

### **Pré-requisitos:**
- **Compilador C** (Ex: GCC)

### **Passos:**
  1. Clone do repositório:
  ```bash
  git clone https://github.com/NicollasBS/rotas-estacoes-espaciais.git
  cd sistema-de-rotas
  ```
  2. Compile o programa:
  ```bash
  gcc main.c -o espacial
  ```
  3. Execute o programa:
  ```bash
  ./espacial
  ```

