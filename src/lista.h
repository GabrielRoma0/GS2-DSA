/*
 * lista.h - lista simplesmente encadeada de servicos, implementada pela
 * equipe (sem biblioteca pronta). Cada no e alocado com malloc e liberado
 * com free.
 */
#ifndef LISTA_H
#define LISTA_H

#include "servico.h"

/* Um no guarda um servico e o endereco do proximo no (NULL no ultimo). */
typedef struct No {
    Servico    dado;
    struct No *prox;
} No;

/* Cabecalho da lista: primeiro no, ultimo no e quantidade. Guardar o
 * ultimo no ('fim') deixa o encadeamento no fim em O(1). */
typedef struct {
    No *inicio;
    No *fim;
    int tamanho;
} Lista;

/* Campo usado na pesquisa por atributo. */
typedef enum { CAMPO_TITULO, CAMPO_CATEGORIA } Campo;

void lista_iniciar(Lista *l);

/* Valida o servico, recusa ID repetido e insere no FIM (mantem a ordem de
 * cadastro). O(n) por causa da checagem de ID repetido. */
Resultado lista_inserir(Lista *l, const Servico *s);

/* Busca sequencial por ID. Devolve o no ou NULL. O(n). */
No *lista_buscar_id(const Lista *l, int id);

/* Pesquisa sequencial por trecho do titulo ou da categoria (sem diferenciar
 * maiuscula e acento). Guarda ate 'max' nos encontrados em 'achados' e
 * devolve quantos encontrou no total. O(n * m). */
int lista_pesquisar(const Lista *l, Campo campo, const char *trecho,
                    const No *achados[], int max);

/* Troca os dados do servico 'id' pelos de 'novo', mantendo o mesmo ID e o
 * mesmo lugar na lista (os ponteiros nao mudam). O(n). */
Resultado lista_atualizar(Lista *l, int id, const Servico *novo);

/* Remove o servico 'id' (no inicio, no meio ou no fim) e libera o no. O(n). */
Resultado lista_remover(Lista *l, int id);

/* Percorre a lista mostrando cada servico. */
void lista_imprimir(const Lista *l, int detalhado);

/* Mostra a estrutura na memoria: endereco de cada no e do proximo. */
void lista_imprimir_estrutura(const Lista *l);

/* Conta servicos por categoria (contagem[i] = categoria CATEGORIAS[i]). */
void lista_contar_categorias(const Lista *l, int contagem[TOTAL_CATEGORIAS]);

/* Maior ID + 1 (sugestao de ID livre). */
int lista_proximo_id(const Lista *l);

/* Carrega servicos de um arquivo texto (formato em dados/servicos.txt).
 * Devolve quantos entraram; 'recusadas' recebe as linhas mal formadas,
 * invalidas ou com ID repetido. Devolve -1 se o arquivo nao abriu. */
int lista_carregar_arquivo(Lista *l, const char *caminho, int *recusadas);

/* Libera todos os nos e deixa a lista vazia. O(n). */
void lista_liberar(Lista *l);

#endif
