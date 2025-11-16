#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10

// ---------------- Estruturas ----------------

// Estrutura da sala (árvore binária da mansão)
typedef struct Sala {
    char nome[50];
    char pista[50];
    char suspeito[50]; // suspeito associado à pista
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura da BST de pistas
typedef struct NodoPista {
    char pista[50];
    struct NodoPista *esquerda;
    struct NodoPista *direita;
} NodoPista;

// Estrutura da tabela hash (lista encadeada para colisões)
typedef struct NodoHash {
    char pista[50];
    char suspeito[50];
    struct NodoHash *prox;
} NodoHash;

// ---------------- Funções de Sala ----------------
Sala* criarSala(const char* nome, const char* pista, const char* suspeito) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    snprintf(nova->nome, sizeof(nova->nome), "%s", nome);
    snprintf(nova->pista, sizeof(nova->pista), "%s", pista);
    snprintf(nova->suspeito, sizeof(nova->suspeito), "%s", suspeito);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ---------------- Funções BST ----------------
NodoPista* criarNodoPista(const char* pista) {
    NodoPista* novo = (NodoPista*) malloc(sizeof(NodoPista));
    snprintf(novo->pista, sizeof(novo->pista), "%s", pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

NodoPista* inserirPista(NodoPista* raiz, const char* pista) {
    if (raiz == NULL) return criarNodoPista(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

void emOrdem(NodoPista* raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        emOrdem(raiz->direita);
    }
}

// ---------------- Funções Hash ----------------
int funcaoHash(const char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) soma += chave[i];
    return soma % TAM_HASH;
}

void inserirNaHash(NodoHash* tabela[], const char* pista, const char* suspeito) {
    int indice = funcaoHash(pista);
    NodoHash* novo = (NodoHash*) malloc(sizeof(NodoHash));
    snprintf(novo->pista, sizeof(novo->pista), "%s", pista);
    snprintf(novo->suspeito, sizeof(novo->suspeito), "%s", suspeito);
    novo->prox = tabela[indice];
    tabela[indice] = novo;
}

void listarHash(NodoHash* tabela[]) {
    printf("\n🔎 Associações pista → suspeito:\n");
    for (int i = 0; i < TAM_HASH; i++) {
        NodoHash* atual = tabela[i];
        while (atual != NULL) {
            printf("%s → %s\n", atual->pista, atual->suspeito);
            atual = atual->prox;
        }
    }
}

void suspeitoMaisCitado(NodoHash* tabela[]) {
    char suspeitos[50][50];
    int contagem[50] = {0};
    int total = 0;

    for (int i = 0; i < TAM_HASH; i++) {
        NodoHash* atual = tabela[i];
        while (atual != NULL) {
            int encontrado = 0;
            for (int j = 0; j < total; j++) {
                if (strcmp(suspeitos[j], atual->suspeito) == 0) {
                    contagem[j]++;
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                snprintf(suspeitos[total], sizeof(suspeitos[total]), "%s", atual->suspeito);
                contagem[total] = 1;
                total++;
            }
            atual = atual->prox;
        }
    }

    int max = 0;
    char culpado[50];
    for (int i = 0; i < total; i++) {
        if (contagem[i] > max) {
            max = contagem[i];
            snprintf(culpado, sizeof(culpado), "%s", suspeitos[i]);
        }
    }

    printf("\n⚖️ Suspeito mais citado: %s (aparece em %d pistas)\n", culpado, max);
}

// ---------------- Exploração ----------------
void explorarSalas(Sala* atual, NodoPista** arvorePistas, NodoHash* tabela[]) {
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("Você encontrou uma pista: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
            inserirNaHash(tabela, atual->pista, atual->suspeito);
        }

        printf("Escolha o caminho:\n");
        printf("e → esquerda | d → direita | p → listar pistas | h → listar hash | s → sair\n");
        scanf(" %c", &opcao);

        if (opcao == 'e') {
            if (atual->esquerda != NULL) atual = atual->esquerda;
            else { printf("Fim do caminho à esquerda!\n"); break; }
        } else if (opcao == 'd') {
            if (atual->direita != NULL) atual = atual->direita;
            else { printf("Fim do caminho à direita!\n"); break; }
        } else if (opcao == 'p') {
            printf("\n📜 Pistas coletadas:\n");
            emOrdem(*arvorePistas);
        } else if (opcao == 'h') {
            listarHash(tabela);
        } else if (opcao == 's') {
            printf("Exploração encerrada.\n");
            break;
        } else {
            printf("Opção inválida!\n");
        }
    }
}

// ---------------- Main ----------------
int main() {
    // Construção da mansão
    Sala* hall = criarSala("Hall de Entrada", "", "");
    Sala* salaEstar = criarSala("Sala de Estar", "Chave dourada", "Sr. Black");
    Sala* biblioteca = criarSala("Biblioteca", "Livro antigo", "Sra. White");
    Sala* cozinha = criarSala("Cozinha", "Copo quebrado", "Sr. Green");
    Sala* jardim = criarSala("Jardim", "Pegada misteriosa", "Srta. Scarlet");

    hall->esquerda = salaEstar;
    hall->direita = biblioteca;
    salaEstar->esquerda = cozinha;
    salaEstar->direita = jardim;

    // Estruturas auxiliares
    NodoPista* arvorePistas = NULL;
    NodoHash* tabela[TAM_HASH] = {NULL};

    // Exploração
    explorarSalas(hall, &arvorePistas, tabela);

    // Resultado final
    listarHash(tabela);
    suspeitoMaisCitado(tabela);

    return 0;
}