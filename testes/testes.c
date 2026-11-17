/*
 * testes.c - testes automaticos da lista, da busca e da ordenacao.
 * Rodar: make test   (compila com AddressSanitizer, que tambem acusa
 * vazamento de memoria e acesso a no ja liberado)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/servico.h"
#include "../src/lista.h"
#include "../src/ordenacao.h"

static int total = 0, falhas = 0;

#define CHECAR(cond, desc) do {                                         \
    total++;                                                            \
    if (cond) { printf("  ok   %s\n", desc); }                          \
    else { falhas++; printf("  FALHOU %s  (%s:%d)\n", desc, __FILE__, __LINE__); } \
} while (0)

static Servico novo(int id, const char *titulo, const char *categoria) {
    Servico s;
    memset(&s, 0, sizeof s);
    s.id = id;
    copiar_texto(s.titulo, titulo, sizeof s.titulo);
    copiar_texto(s.categoria, categoria, sizeof s.categoria);
    copiar_texto(s.descricao, "descricao de teste", sizeof s.descricao);
    copiar_texto(s.local, "local de teste", sizeof s.local);
    return s;
}

/* Confere se inicio, fim, tamanho e os ponteiros estao coerentes. */
static int integra(const Lista *l) {
    int n = 0;
    const No *p = l->inicio, *ultimo = NULL;
    while (p != NULL) { ultimo = p; p = p->prox; n++; }
    return n == l->tamanho && ultimo == l->fim &&
           (l->tamanho == 0) == (l->inicio == NULL);
}

static void ids(const Lista *l, char *saida, size_t tam) {
    const No *p;
    saida[0] = '\0';
    for (p = l->inicio; p != NULL; p = p->prox) {
        char b[16];
        snprintf(b, sizeof b, "%s%d", saida[0] ? "," : "", p->dado.id);
        strncat(saida, b, tam - strlen(saida) - 1);
    }
}

static void teste_lista_vazia(void) {
    Lista l; int cont[TOTAL_CATEGORIAS]; const No *a[4]; Servico s = novo(1, "x", "Transporte");
    printf("\n[lista vazia]\n");
    lista_iniciar(&l);
    CHECAR(l.tamanho == 0 && l.inicio == NULL && l.fim == NULL, "lista comeca vazia");
    CHECAR(lista_buscar_id(&l, 1) == NULL, "buscar em lista vazia devolve NULL");
    CHECAR(lista_remover(&l, 1) == ERRO_LISTA_VAZIA, "remover em lista vazia -> ERRO_LISTA_VAZIA");
    CHECAR(lista_atualizar(&l, 1, &s) == ERRO_LISTA_VAZIA, "atualizar em lista vazia -> ERRO_LISTA_VAZIA");
    CHECAR(lista_pesquisar(&l, CAMPO_TITULO, "x", a, 4) == 0, "pesquisar em lista vazia -> 0");
    ordenar_merge(&l, POR_TITULO);
    CHECAR(integra(&l) && l.inicio == NULL, "merge sort em lista vazia nao quebra");
    ordenar_insercao(&l, POR_TITULO);
    CHECAR(integra(&l) && l.inicio == NULL, "insertion sort em lista vazia nao quebra");
    lista_contar_categorias(&l, cont);
    CHECAR(cont[0] == 0 && cont[4] == 0, "contagem por categoria zerada");
    CHECAR(lista_proximo_id(&l) == 1, "proximo ID sugerido e 1");
    lista_liberar(&l);
    CHECAR(integra(&l), "liberar lista vazia nao quebra");
}

static void teste_insercao_validacao(void) {
    Lista l; Servico s; char seq[64];
    printf("\n[insercao e validacao]\n");
    lista_iniciar(&l);
    s = novo(10, "A", "Transporte"); CHECAR(lista_inserir(&l, &s) == OK, "insere id 10");
    s = novo(20, "B", "Beneficio");  CHECAR(lista_inserir(&l, &s) == OK, "insere id 20");
    s = novo(30, "C", "Calcada");    CHECAR(lista_inserir(&l, &s) == OK, "insere id 30");
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "10,20,30") == 0 && integra(&l), "insercao no fim mantem a ordem de cadastro");

    s = novo(20, "Outro", "Transporte");
    CHECAR(lista_inserir(&l, &s) == ERRO_ID_DUPLICADO && l.tamanho == 3, "ID duplicado e recusado e a lista nao muda");
    s = novo(0, "Zero", "Transporte");
    CHECAR(lista_inserir(&l, &s) == ERRO_ID_INVALIDO, "ID 0 e recusado");
    s = novo(-5, "Neg", "Transporte");
    CHECAR(lista_inserir(&l, &s) == ERRO_ID_INVALIDO, "ID negativo e recusado");
    s = novo(40, "   ", "Transporte");
    CHECAR(lista_inserir(&l, &s) == ERRO_CAMPO_VAZIO, "titulo so com espacos e recusado");
    s = novo(40, "D", "Lazer");
    CHECAR(lista_inserir(&l, &s) == ERRO_CATEGORIA, "categoria fora da lista e recusada");
    s = novo(40, "D", "beneficio");
    CHECAR(lista_inserir(&l, &s) == OK, "categoria aceita sem diferenciar maiuscula");
    CHECAR(l.tamanho == 4 && integra(&l), "tamanho final 4 e ponteiros coerentes");
    CHECAR(lista_proximo_id(&l) == 41, "proximo ID sugerido = maior + 1");
    lista_liberar(&l);
    CHECAR(l.tamanho == 0 && l.inicio == NULL && l.fim == NULL, "liberar zera a lista");
}

static void teste_busca(void) {
    Lista l; Servico s; const No *a[10]; int n;
    printf("\n[busca]\n");
    lista_iniciar(&l);
    s = novo(1, "Reclamar de motorista de ônibus", "Atendimento"); lista_inserir(&l, &s);
    s = novo(2, "Calçada pública quebrada", "Calcada");             lista_inserir(&l, &s);
    s = novo(3, "Bilhete Único Especial", "Transporte");            lista_inserir(&l, &s);
    s = novo(4, "Ouvidoria da SPTrans: ônibus", "Atendimento");     lista_inserir(&l, &s);

    CHECAR(lista_buscar_id(&l, 3) != NULL && lista_buscar_id(&l, 3)->dado.id == 3, "busca por ID existente");
    CHECAR(lista_buscar_id(&l, 1) == l.inicio, "busca o primeiro no");
    CHECAR(lista_buscar_id(&l, 4) == l.fim, "busca o ultimo no");
    CHECAR(lista_buscar_id(&l, 99) == NULL, "ID inexistente devolve NULL");

    n = lista_pesquisar(&l, CAMPO_TITULO, "ONIBUS", a, 10);
    CHECAR(n == 2 && a[0]->dado.id == 1 && a[1]->dado.id == 4, "\"ONIBUS\" acha \"ônibus\" (sem acento e sem maiuscula)");
    n = lista_pesquisar(&l, CAMPO_TITULO, "calcada", a, 10);
    CHECAR(n == 1 && a[0]->dado.id == 2, "\"calcada\" acha \"Calçada\"");
    n = lista_pesquisar(&l, CAMPO_CATEGORIA, "atend", a, 10);
    CHECAR(n == 2, "pesquisa por trecho da categoria");
    CHECAR(lista_pesquisar(&l, CAMPO_TITULO, "aeroporto", a, 10) == 0, "pesquisa sem resultado devolve 0");
    CHECAR(lista_pesquisar(&l, CAMPO_TITULO, "", a, 10) == 0, "pesquisa com texto vazio nao casa nada");
    n = lista_pesquisar(&l, CAMPO_TITULO, "a", a, 1);
    CHECAR(n == 4, "conta todos os achados mesmo com limite menor no vetor");
    lista_liberar(&l);
}

static void teste_remocao(void) {
    Lista l; Servico s; char seq[64];
    printf("\n[remocao: inicio, meio, fim]\n");
    lista_iniciar(&l);
    for (int i = 1; i <= 5; i++) { s = novo(i, "T", "Transporte"); lista_inserir(&l, &s); }

    CHECAR(lista_remover(&l, 1) == OK, "remove o primeiro (id 1)");
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "2,3,4,5") == 0 && l.inicio->dado.id == 2 && integra(&l), "inicio aponta para o id 2");
    CHECAR(lista_remover(&l, 3) == OK, "remove do meio (id 3)");
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "2,4,5") == 0 && integra(&l), "lista religada: 2,4,5");
    CHECAR(lista_remover(&l, 5) == OK, "remove o ultimo (id 5)");
    CHECAR(l.fim != NULL && l.fim->dado.id == 4 && integra(&l), "fim passa a ser o id 4");
    s = novo(6, "T", "Transporte");
    CHECAR(lista_inserir(&l, &s) == OK && l.fim->dado.id == 6 && integra(&l), "inserir depois de remover o fim continua certo");
    CHECAR(lista_remover(&l, 99) == ERRO_NAO_ENCONTRADO && l.tamanho == 3, "remover inexistente nao muda nada");
    lista_remover(&l, 2); lista_remover(&l, 4); lista_remover(&l, 6);
    CHECAR(l.tamanho == 0 && l.inicio == NULL && l.fim == NULL, "removendo tudo a lista fica vazia");
    lista_liberar(&l);
}

static void teste_atualizacao(void) {
    Lista l; Servico s, mod; No *antes_prox;
    printf("\n[atualizacao]\n");
    lista_iniciar(&l);
    s = novo(1, "Antigo", "Transporte"); lista_inserir(&l, &s);
    s = novo(2, "Outro", "Beneficio");   lista_inserir(&l, &s);
    antes_prox = l.inicio->prox;

    mod = novo(777, "Novo titulo", "Calcada");
    CHECAR(lista_atualizar(&l, 1, &mod) == OK, "atualiza servico existente");
    CHECAR(strcmp(l.inicio->dado.titulo, "Novo titulo") == 0 && l.inicio->dado.id == 1,
           "titulo muda e o ID continua 1 (nao vira 777)");
    CHECAR(l.inicio->prox == antes_prox && integra(&l), "ponteiros nao mudam na atualizacao");
    mod = novo(1, "", "Calcada");
    CHECAR(lista_atualizar(&l, 1, &mod) == ERRO_CAMPO_VAZIO &&
           strcmp(l.inicio->dado.titulo, "Novo titulo") == 0, "dado invalido e recusado e o registro fica intacto");
    mod = novo(1, "X", "Inexistente");
    CHECAR(lista_atualizar(&l, 1, &mod) == ERRO_CATEGORIA, "categoria invalida na atualizacao e recusada");
    CHECAR(lista_atualizar(&l, 50, &mod) == ERRO_NAO_ENCONTRADO, "atualizar ID inexistente");
    lista_liberar(&l);
}

static void teste_ordenacao(void) {
    Lista l; Servico s; char seq[64];
    const char *titulos[] = { "Ouvidoria", "Bilhete Único", "Atende+", "Calçada", "Central 156", "Ônibus" };
    const char *cats[]    = { "Atendimento", "Transporte", "Transporte", "Calcada", "Atendimento", "Atendimento" };
    printf("\n[ordenacao]\n");
    lista_iniciar(&l);
    for (int i = 0; i < 6; i++) { s = novo(60 - i * 10, titulos[i], cats[i]); lista_inserir(&l, &s); }

    ordenar_merge(&l, POR_TITULO);
    ids(&l, seq, sizeof seq);
    /* Atende+(40) Bilhete(50) Calcada(30) Central(20) Onibus(10) Ouvidoria(60) */
    CHECAR(strcmp(seq, "40,50,30,20,10,60") == 0, "merge por titulo (\"Ônibus\" fica entre C e Ou)");
    CHECAR(integra(&l) && l.fim->dado.id == 60, "fim atualizado depois do merge");

    ordenar_insercao(&l, POR_ID);
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "10,20,30,40,50,60") == 0 && integra(&l), "insertion por ID");

    ordenar_merge(&l, POR_CATEGORIA);
    ids(&l, seq, sizeof seq);
    /* Atendimento: Central(20) Onibus(10) Ouvidoria(60) | Calcada(30) | Transporte: Atende(40) Bilhete(50) */
    CHECAR(strcmp(seq, "20,10,60,30,40,50") == 0, "merge por categoria desempata pelo titulo");

    ordenar_insercao(&l, POR_TITULO);
    CHECAR(lista_esta_ordenada(&l, POR_TITULO) && integra(&l), "insertion por titulo deixa em ordem");
    lista_liberar(&l);

    /* um so elemento */
    lista_iniciar(&l);
    s = novo(1, "Unico", "Transporte"); lista_inserir(&l, &s);
    ordenar_merge(&l, POR_TITULO);
    CHECAR(l.tamanho == 1 && l.inicio == l.fim && integra(&l), "ordenar lista de 1 no");
    lista_liberar(&l);

    /* estabilidade: titulos iguais mantem a ordem de cadastro */
    lista_iniciar(&l);
    s = novo(3, "Igual", "Transporte"); lista_inserir(&l, &s);
    s = novo(1, "Igual", "Transporte"); lista_inserir(&l, &s);
    s = novo(2, "Antes", "Transporte"); lista_inserir(&l, &s);
    ordenar_merge(&l, POR_TITULO);
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "2,3,1") == 0, "merge e estavel (3 continua antes de 1)");
    ordenar_insercao(&l, POR_ID); ordenar_insercao(&l, POR_TITULO);
    ids(&l, seq, sizeof seq);
    CHECAR(strcmp(seq, "2,1,3") == 0, "insertion e estavel");
    lista_liberar(&l);
}

/* Mede as comparacoes com n grande: evidencia pratica do Big-O. */
static void teste_complexidade(void) {
    const int tamanhos[] = { 100, 1000, 4000 };
    printf("\n[complexidade: comparacoes medidas, dados embaralhados com semente fixa]\n");
    printf("  %6s  %14s  %14s  %12s  %12s\n", "n", "Merge Sort", "Insertion", "n*log2(n)", "n^2/4");
    srand(2026);
    for (int t = 0; t < 3; t++) {
        int n = tamanhos[t];
        int *perm = (int *)malloc(sizeof(int) * (size_t)n);
        Lista a, b; Servico s;
        unsigned long cm, ci;
        double lg = 0; for (int x = n; x > 1; x /= 2) lg += 1;
        for (int i = 0; i < n; i++) perm[i] = i + 1;
        for (int i = n - 1; i > 0; i--) { int j = rand() % (i + 1); int x = perm[i]; perm[i] = perm[j]; perm[j] = x; }
        lista_iniciar(&a); lista_iniciar(&b);
        for (int i = 0; i < n; i++) { s = novo(perm[i], "T", "Transporte"); lista_inserir(&a, &s); lista_inserir(&b, &s); }
        ordenar_merge(&a, POR_ID);    cm = ordenacao_comparacoes;
        ordenar_insercao(&b, POR_ID); ci = ordenacao_comparacoes;
        printf("  %6d  %14lu  %14lu  %12.0f  %12.0f\n", n, cm, ci, n * lg, (double)n * n / 4);
        CHECAR(lista_esta_ordenada(&a, POR_ID) && lista_esta_ordenada(&b, POR_ID) && integra(&a) && integra(&b),
               "as duas ordenam corretamente");
        if (n >= 1000) CHECAR(ci > 10 * cm, "insertion faz muito mais comparacoes que merge com n grande");
        lista_liberar(&a); lista_liberar(&b); free(perm);
    }
}

static void teste_arquivo(void) {
    Lista l; int rec, n;
    printf("\n[carga do arquivo]\n");
    lista_iniciar(&l);
    n = lista_carregar_arquivo(&l, "dados/servicos.txt", &rec);
    CHECAR(n == 14 && rec == 0 && l.tamanho == 14, "carrega os 14 servicos do Mapiii");
    n = lista_carregar_arquivo(&l, "dados/servicos.txt", &rec);
    CHECAR(n == 0 && rec == 14 && l.tamanho == 14, "carregar de novo recusa os 14 IDs repetidos");
    CHECAR(lista_carregar_arquivo(&l, "dados/nao-existe.txt", &rec) == -1, "arquivo inexistente devolve -1");
    lista_liberar(&l);
}

static void teste_linha(void) {
    Servico s;
    printf("\n[leitura de linha do arquivo]\n");
    CHECAR(servico_de_linha("7|Titulo|Calcada|desc||local|https://x\n", &s) && s.id == 7 &&
           s.requisitos[0] == '\0' && strcmp(s.fonte, "https://x") == 0, "linha valida com campo vazio");
    CHECAR(!servico_de_linha("7|Titulo|Calcada|desc|local|https://x", &s), "linha com 6 campos e recusada");
    CHECAR(!servico_de_linha("abc|Titulo|Calcada|desc|req|local|f", &s), "ID nao numerico e recusado");
    CHECAR(!servico_de_linha("1|a|b|c|d|e|f|g", &s), "linha com 8 campos e recusada");
}

int main(void) {
    printf("Testes do Citizen Assist AI (lista encadeada, busca e ordenacao)\n");
    teste_lista_vazia();
    teste_insercao_validacao();
    teste_busca();
    teste_remocao();
    teste_atualizacao();
    teste_ordenacao();
    teste_complexidade();
    teste_arquivo();
    teste_linha();
    printf("\n%d testes, %d falha(s).\n", total, falhas);
    return falhas == 0 ? 0 : 1;
}
