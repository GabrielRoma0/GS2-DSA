/*
 * ordenacao.h - algoritmos de ordenacao escritos pela equipe, que trabalham
 * direto nos ponteiros da lista encadeada (nao copiam para vetor e nao usam
 * qsort).
 */
#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "lista.h"

typedef enum { POR_TITULO, POR_CATEGORIA, POR_ID } Criterio;

/* Quantas comparacoes a ultima ordenacao fez (para comparar os algoritmos). */
extern unsigned long ordenacao_comparacoes;

/* Compara dois servicos pelo criterio. <0, 0 ou >0 como strcmp.
 * POR_CATEGORIA desempata pelo titulo. */
int comparar_servicos(const Servico *a, const Servico *b, Criterio c);

/* Merge Sort na lista: O(n log n) em qualquer caso, estavel. */
void ordenar_merge(Lista *l, Criterio c);

/* Insertion Sort na lista: O(n^2) no pior caso, O(n) se ja ordenada, estavel. */
void ordenar_insercao(Lista *l, Criterio c);

/* 1 se a lista esta em ordem pelo criterio (usado nos testes). */
int lista_esta_ordenada(const Lista *l, Criterio c);

#endif
