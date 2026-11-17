/*
 * main.c - menu interativo do Citizen Assist AI (DSA, GS 2026.2).
 *
 * Recorte: servicos e direitos para pessoas com deficiencia em Sao Paulo.
 * Os exemplos de dados/servicos.txt vem das fichas conferidas do Mapiii,
 * cada uma com o link oficial de onde a informacao foi tirada.
 *
 * Uso: ./citizen [arquivo_de_carga]   (padrao: dados/servicos.txt)
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L  /* para fileno() e isatty() com -std=c11 */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#include "servico.h"
#include "lista.h"
#include "ordenacao.h"

#define ARQUIVO_PADRAO "dados/servicos.txt"
#define MAX_ACHADOS 100

/* Quando a entrada vem de arquivo (./citizen < roteiro.txt), repetimos o que
 * foi "digitado" para a saida ficar legivel como evidencia de teste. */
static int ecoar_entrada = 0;
/* Vira 1 quando a entrada acaba (Ctrl+D / fim do roteiro): o programa sai
 * limpo, liberando a memoria, em vez de entrar em laco infinito. */
static int fim_da_entrada = 0;

/* ---------------------------------------------------------------------
 * Leitura segura do teclado (fgets, nunca scanf("%s"))
 * ------------------------------------------------------------------- */

/* Le uma linha. Devolve 1 se leu, 0 se a entrada acabou ou se o texto
 * passou do tamanho do campo (nesse caso avisa e descarta o resto). */
static int ler_linha(const char *pergunta, char *buf, size_t tam) {
    size_t n;
    printf("%s", pergunta);
    fflush(stdout);
    if (fgets(buf, (int)tam, stdin) == NULL) {
        fim_da_entrada = 1;
        buf[0] = '\0';
        printf("\n");
        return 0;
    }
    n = strlen(buf);
    if (n > 0 && buf[n - 1] != '\n' && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { /* descarta o excesso */ }
        if (ecoar_entrada) printf("%s...\n", buf);
        printf("  ! Texto longo demais (maximo de %lu caracteres).\n", (unsigned long)(tam - 2));
        buf[0] = '\0';
        return 0;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    if (ecoar_entrada) printf("%s\n", buf);
    return 1;
}

/* 1 se 's' e um inteiro valido (com espacos em volta permitidos). */
static int texto_para_int(const char *s, int *valor) {
    char *fim;
    long v;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return 0;
    errno = 0;
    v = strtol(s, &fim, 10);
    while (isspace((unsigned char)*fim)) fim++;
    if (*fim != '\0' || errno == ERANGE || v < -999999 || v > 999999) return 0;
    *valor = (int)v;
    return 1;
}

/* Le um inteiro. Devolve 1 se valido; 0 (com aviso) se nao. */
static int ler_inteiro(const char *pergunta, int *valor) {
    char buf[64];
    if (!ler_linha(pergunta, buf, sizeof buf)) return 0;
    if (!texto_para_int(buf, valor)) {
        printf("  ! Entrada invalida: digite um numero inteiro.\n");
        return 0;
    }
    return 1;
}

/* Le uma opcao entre min e max. */
static int ler_opcao(const char *pergunta, int min, int max, int *valor) {
    if (!ler_inteiro(pergunta, valor)) return 0;
    if (*valor < min || *valor > max) {
        printf("  ! Opcao invalida: escolha de %d a %d.\n", min, max);
        return 0;
    }
    return 1;
}

static void mostrar_categorias(void) {
    for (int i = 0; i < TOTAL_CATEGORIAS; i++) {
        printf("    %d - %s\n", i + 1, CATEGORIAS[i]);
    }
}

/* Le a categoria pelo numero. 'permitir_manter' aceita Enter (devolve -1). */
static int ler_categoria(int permitir_manter, int *indice) {
    char buf[16];
    int v;
    mostrar_categorias();
    if (!ler_linha(permitir_manter ? "  Categoria (numero, Enter mantem): "
                                   : "  Categoria (numero): ", buf, sizeof buf)) return 0;
    if (permitir_manter && buf[0] == '\0') { *indice = -1; return 1; }
    if (!texto_para_int(buf, &v) || v < 1 || v > TOTAL_CATEGORIAS) {
        printf("  ! Categoria invalida: escolha de 1 a %d.\n", TOTAL_CATEGORIAS);
        return 0;
    }
    *indice = v - 1;
    return 1;
}

static int lista_vazia_avisar(const Lista *l) {
    if (l->tamanho == 0) {
        printf("  ! %s\n", resultado_texto(ERRO_LISTA_VAZIA));
        return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------
 * Opcoes do menu
 * ------------------------------------------------------------------- */

static void opcao_cadastrar(Lista *l) {
    Servico s;
    char buf[32], pergunta[64];
    int sugestao = lista_proximo_id(l), cat;
    Resultado r;

    memset(&s, 0, sizeof s);
    printf("\n== Cadastrar servico ==\n");
    snprintf(pergunta, sizeof pergunta, "  ID (Enter = %d): ", sugestao);
    if (!ler_linha(pergunta, buf, sizeof buf)) return;
    if (buf[0] == '\0') {
        s.id = sugestao;
    } else if (!texto_para_int(buf, &s.id)) {
        printf("  ! Entrada invalida: digite um numero inteiro.\n");
        return;
    }
    /* confere o ID antes de pedir o resto, para nao fazer a pessoa digitar a toa */
    if (s.id <= 0) { printf("  ! %s\n", resultado_texto(ERRO_ID_INVALIDO)); return; }
    if (lista_buscar_id(l, s.id) != NULL) { printf("  ! %s\n", resultado_texto(ERRO_ID_DUPLICADO)); return; }

    if (!ler_linha("  Titulo: ", s.titulo, sizeof s.titulo)) return;
    if (!ler_categoria(0, &cat)) return;
    copiar_texto(s.categoria, CATEGORIAS[cat], sizeof s.categoria);
    if (!ler_linha("  Descricao: ", s.descricao, sizeof s.descricao)) return;
    if (!ler_linha("  Requisitos/documentos (opcional): ", s.requisitos, sizeof s.requisitos)) return;
    if (!ler_linha("  Onde pedir: ", s.local, sizeof s.local)) return;
    if (!ler_linha("  Fonte (link oficial, opcional): ", s.fonte, sizeof s.fonte)) return;

    r = lista_inserir(l, &s);
    if (r == OK) printf("  Servico %d cadastrado. Total: %d.\n", s.id, l->tamanho);
    else         printf("  ! %s\n", resultado_texto(r));
}

static void opcao_consultar(const Lista *l) {
    int id;
    No *no;
    printf("\n== Consultar por ID ==\n");
    if (lista_vazia_avisar(l)) return;
    if (!ler_inteiro("  ID: ", &id)) return;
    no = lista_buscar_id(l, id);
    if (no == NULL) printf("  ! %s (ID %d)\n", resultado_texto(ERRO_NAO_ENCONTRADO), id);
    else            servico_imprimir(&no->dado, 1);
}

static void opcao_pesquisar(const Lista *l) {
    const No *achados[MAX_ACHADOS];
    char trecho[TAM_TITULO];
    int campo, total;
    printf("\n== Pesquisar ==\n");
    if (lista_vazia_avisar(l)) return;
    printf("    1 - por titulo\n    2 - por categoria\n");
    if (!ler_opcao("  Pesquisar por: ", 1, 2, &campo)) return;
    if (!ler_linha("  Texto (sem precisar de acento): ", trecho, sizeof trecho)) return;
    if (trecho[0] == '\0') { printf("  ! Digite algum texto para pesquisar.\n"); return; }

    total = lista_pesquisar(l, campo == 1 ? CAMPO_TITULO : CAMPO_CATEGORIA,
                            trecho, achados, MAX_ACHADOS);
    if (total == 0) {
        printf("  Nenhum servico encontrado para \"%s\".\n", trecho);
        return;
    }
    printf("  %d servico(s) encontrado(s):\n", total);
    for (int i = 0; i < total && i < MAX_ACHADOS; i++) servico_imprimir(&achados[i]->dado, 0);
}

/* Le um campo de texto na atualizacao: Enter mantem o valor atual. */
static int ler_campo_novo(const char *nome, char *destino, size_t tam, const char *atual) {
    char pergunta[96];
    char *buf = (char *)malloc(tam);
    int ok;
    if (buf == NULL) { printf("  ! %s\n", resultado_texto(ERRO_MEMORIA)); return 0; }
    snprintf(pergunta, sizeof pergunta, "  %s (Enter mantem): ", nome);
    ok = ler_linha(pergunta, buf, tam);
    if (ok) copiar_texto(destino, buf[0] != '\0' ? buf : atual, tam);
    free(buf);
    return ok;
}

static void opcao_atualizar(Lista *l) {
    int id, cat;
    No *no;
    Servico novo;
    Resultado r;

    printf("\n== Atualizar servico ==\n");
    if (lista_vazia_avisar(l)) return;
    if (!ler_inteiro("  ID do servico: ", &id)) return;
    no = lista_buscar_id(l, id);
    if (no == NULL) { printf("  ! %s (ID %d)\n", resultado_texto(ERRO_NAO_ENCONTRADO), id); return; }
    servico_imprimir(&no->dado, 1);

    novo = no->dado;   /* comeca igual; so muda o que for digitado */
    if (!ler_campo_novo("Titulo", novo.titulo, sizeof novo.titulo, no->dado.titulo)) return;
    if (!ler_categoria(1, &cat)) return;
    if (cat >= 0) copiar_texto(novo.categoria, CATEGORIAS[cat], sizeof novo.categoria);
    if (!ler_campo_novo("Descricao", novo.descricao, sizeof novo.descricao, no->dado.descricao)) return;
    if (!ler_campo_novo("Requisitos", novo.requisitos, sizeof novo.requisitos, no->dado.requisitos)) return;
    if (!ler_campo_novo("Onde pedir", novo.local, sizeof novo.local, no->dado.local)) return;
    if (!ler_campo_novo("Fonte", novo.fonte, sizeof novo.fonte, no->dado.fonte)) return;

    r = lista_atualizar(l, id, &novo);
    if (r == OK) { printf("  Servico %d atualizado (o ID nao muda).\n", id); servico_imprimir(&no->dado, 0); }
    else         printf("  ! %s Nada foi alterado.\n", resultado_texto(r));
}

static void opcao_excluir(Lista *l) {
    int id;
    char resp[8];
    No *no;
    Resultado r;

    printf("\n== Excluir servico ==\n");
    if (lista_vazia_avisar(l)) return;
    if (!ler_inteiro("  ID do servico: ", &id)) return;
    no = lista_buscar_id(l, id);
    if (no == NULL) { printf("  ! %s (ID %d)\n", resultado_texto(ERRO_NAO_ENCONTRADO), id); return; }
    servico_imprimir(&no->dado, 0);
    if (!ler_linha("  Confirma a exclusao? (s/n): ", resp, sizeof resp)) return;
    if (resp[0] != 's' && resp[0] != 'S') { printf("  Exclusao cancelada.\n"); return; }

    r = lista_remover(l, id);
    if (r == OK) printf("  Servico %d excluido. Total: %d.\n", id, l->tamanho);
    else         printf("  ! %s\n", resultado_texto(r));
}

static void opcao_ordenar(Lista *l) {
    static const char *const NOMES_CRITERIO[] = { "titulo (A-Z)", "categoria e titulo", "ID" };
    int criterio, algoritmo;

    printf("\n== Ordenar e listar ==\n");
    if (lista_vazia_avisar(l)) return;
    printf("    1 - por titulo (A-Z)\n    2 - por categoria (e titulo)\n    3 - por ID\n");
    if (!ler_opcao("  Criterio: ", 1, 3, &criterio)) return;
    printf("    1 - Merge Sort     (O(n log n))\n    2 - Insertion Sort (O(n^2))\n");
    if (!ler_opcao("  Algoritmo: ", 1, 2, &algoritmo)) return;

    if (algoritmo == 1) ordenar_merge(l, (Criterio)(criterio - 1));
    else                ordenar_insercao(l, (Criterio)(criterio - 1));

    printf("  Ordenado por %s com %s: %lu comparacoes para %d servicos.\n",
           NOMES_CRITERIO[criterio - 1], algoritmo == 1 ? "Merge Sort" : "Insertion Sort",
           ordenacao_comparacoes, l->tamanho);
    lista_imprimir(l, 0);
}

static void opcao_analise(const Lista *l) {
    int contagem[TOTAL_CATEGORIAS], maior = 0;
    printf("\n== Analise ==\n");
    if (lista_vazia_avisar(l)) return;
    lista_contar_categorias(l, contagem);
    printf("  Total de servicos: %d\n  Por categoria:\n", l->tamanho);
    for (int i = 0; i < TOTAL_CATEGORIAS; i++) {
        printf("    %-12s %3d  ", CATEGORIAS[i], contagem[i]);
        for (int j = 0; j < contagem[i]; j++) putchar('#');
        printf("\n");
        if (contagem[i] > contagem[maior]) maior = i;
    }
    printf("  Categoria com mais servicos: %s (%d de %d, %.0f%%)\n", CATEGORIAS[maior],
           contagem[maior], l->tamanho, 100.0 * contagem[maior] / l->tamanho);
}

static void opcao_listar(const Lista *l) {
    printf("\n== Todos os servicos (ordem atual da lista) ==\n");
    if (lista_vazia_avisar(l)) return;
    lista_imprimir(l, 0);
    printf("  Total: %d. Use a opcao 2 para ver os detalhes de um servico.\n", l->tamanho);
}

static void opcao_estrutura(const Lista *l) {
    printf("\n== Lista encadeada na memoria ==\n");
    lista_imprimir_estrutura(l);
}

static void opcao_carregar(Lista *l, const char *caminho) {
    int recusadas, inseridos;
    printf("\n== Carregar exemplos ==\n");
    inseridos = lista_carregar_arquivo(l, caminho, &recusadas);
    if (inseridos < 0) {
        printf("  ! Nao foi possivel abrir \"%s\". Rode o programa a partir da pasta do projeto.\n", caminho);
        return;
    }
    printf("  %d servico(s) carregado(s) de %s; %d linha(s) recusada(s) (ID repetido ou dado invalido). Total: %d.\n",
           inseridos, caminho, recusadas, l->tamanho);
}

static void mostrar_menu(int tamanho) {
    printf("\n==============================================================\n");
    printf(" CITIZEN ASSIST AI - servicos para pessoas com deficiencia (SP)\n");
    printf(" Prototipo de estudo, nao e canal oficial. Confirme na fonte.\n");
    printf("==============================================================\n");
    printf("  1 - Cadastrar            6 - Ordenar e listar\n");
    printf("  2 - Consultar por ID     7 - Analise (indicadores)\n");
    printf("  3 - Pesquisar            8 - Listar todos\n");
    printf("  4 - Atualizar            9 - Carregar exemplos\n");
    printf("  5 - Excluir             10 - Ver a lista na memoria\n");
    printf("  0 - Sair\n");
    printf("  [%d servico(s) na lista]\n", tamanho);
}

int main(int argc, char *argv[]) {
    Lista lista;
    const char *arquivo = (argc > 1) ? argv[1] : ARQUIVO_PADRAO;
    int opcao;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   /* acentos dos dados aparecem certos no Windows */
    SetConsoleCP(CP_UTF8);
#endif
    ecoar_entrada = !isatty(fileno(stdin));
    lista_iniciar(&lista);

    for (;;) {
        mostrar_menu(lista.tamanho);
        if (!ler_inteiro("  Opcao: ", &opcao)) {
            if (fim_da_entrada) break;
            continue;
        }
        switch (opcao) {
            case 1:  opcao_cadastrar(&lista); break;
            case 2:  opcao_consultar(&lista); break;
            case 3:  opcao_pesquisar(&lista); break;
            case 4:  opcao_atualizar(&lista); break;
            case 5:  opcao_excluir(&lista); break;
            case 6:  opcao_ordenar(&lista); break;
            case 7:  opcao_analise(&lista); break;
            case 8:  opcao_listar(&lista); break;
            case 9:  opcao_carregar(&lista, arquivo); break;
            case 10: opcao_estrutura(&lista); break;
            case 0:  break;
            default: printf("  ! Opcao invalida: escolha de 0 a 10.\n");
        }
        if (opcao == 0 || fim_da_entrada) break;
    }

    lista_liberar(&lista);
    printf("\nMemoria liberada (%d nos na lista). Ate logo!\n", lista.tamanho);
    return 0;
}
