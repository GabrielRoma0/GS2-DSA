/*
 * ordenacao.c - Merge Sort e Insertion Sort sobre a lista encadeada.
 *
 * Os dois algoritmos so trocam ponteiros 'prox': os servicos (cerca de 2 KB
 * cada) nunca sao copiados, entao ordenar e barato mesmo com nos grandes.
 */
#include <stddef.h>
#include "ordenacao.h"

unsigned long ordenacao_comparacoes = 0;

int comparar_servicos(const Servico *a, const Servico *b, Criterio c) {
    int r;
    ordenacao_comparacoes++;
    switch (c) {
        case POR_ID:
            return (a->id > b->id) - (a->id < b->id);
        case POR_CATEGORIA:
            r = texto_comparar(a->categoria, b->categoria);
            return r != 0 ? r : texto_comparar(a->titulo, b->titulo);
        case POR_TITULO:
        default:
            return texto_comparar(a->titulo, b->titulo);
    }
}

/* Depois de reordenar, acha de novo o ultimo no (o 'fim' mudou). O(n). */
static void recalcular_fim(Lista *l) {
    No *atual = l->inicio;
    l->fim = NULL;
    while (atual != NULL) {
        l->fim = atual;
        atual = atual->prox;
    }
}

/* ---------------------------------------------------------------------
 * MERGE SORT (dividir e conquistar)
 *
 * 1. Dividir: acha o meio da lista com dois ponteiros - o "lento" anda 1
 *    no por vez e o "rapido" anda 2. Quando o rapido chega ao fim, o lento
 *    esta no meio. Corta a lista ali, gerando duas metades.
 * 2. Conquistar: ordena cada metade chamando o proprio merge sort
 *    (recursao). Lista com 0 ou 1 no ja esta ordenada (caso base).
 * 3. Intercalar: junta as duas metades ordenadas numa so, sempre pegando o
 *    menor dos dois primeiros nos.
 *
 * Sao log2(n) niveis de divisao e cada nivel intercala n nos: O(n log n).
 * ------------------------------------------------------------------- */
static No *dividir_ao_meio(No *cabeca) {
    No *lento = cabeca;
    No *rapido = cabeca->prox;
    No *segunda;
    while (rapido != NULL && rapido->prox != NULL) {
        lento = lento->prox;
        rapido = rapido->prox->prox;
    }
    segunda = lento->prox;
    lento->prox = NULL;   /* corta: a primeira metade termina em 'lento' */
    return segunda;
}

static No *intercalar(No *a, No *b, Criterio c) {
    No sentinela;         /* no "falso" so para simplificar o encadeamento */
    No *ultimo = &sentinela;
    sentinela.prox = NULL;

    while (a != NULL && b != NULL) {
        /* '<= 0' pega primeiro da metade 'a' em caso de empate: isso
         * mantem a ordem original dos iguais (ordenacao estavel). */
        if (comparar_servicos(&a->dado, &b->dado, c) <= 0) {
            ultimo->prox = a;
            a = a->prox;
        } else {
            ultimo->prox = b;
            b = b->prox;
        }
        ultimo = ultimo->prox;
    }
    ultimo->prox = (a != NULL) ? a : b;   /* o que sobrou ja esta em ordem */
    return sentinela.prox;
}

static No *merge_sort(No *cabeca, Criterio c) {
    No *segunda;
    if (cabeca == NULL || cabeca->prox == NULL) return cabeca;  /* caso base */
    segunda = dividir_ao_meio(cabeca);
    return intercalar(merge_sort(cabeca, c), merge_sort(segunda, c), c);
}

void ordenar_merge(Lista *l, Criterio c) {
    ordenacao_comparacoes = 0;
    l->inicio = merge_sort(l->inicio, c);
    recalcular_fim(l);
}

/* ---------------------------------------------------------------------
 * INSERTION SORT
 *
 * Tira os nos da lista original um por um e encaixa cada um na posicao
 * certa de uma nova lista "ordenada", como quem organiza cartas na mao.
 * Para achar a posicao, percorre a lista ordenada desde o inicio.
 *
 * Antes de percorrer, testa dois atalhos: encaixar no fim (>= ultimo) e
 * encaixar no inicio (< primeiro). Por isso lista ja em ordem OU em ordem
 * inversa custa no maximo 2 comparacoes por no -> O(n) (melhor caso).
 * Pior caso e caso medio (dados misturados): cada no pode percorrer boa
 * parte dos anteriores: ate 1 + 2 + ... + (n-1) = n(n-1)/2 -> O(n^2).
 * ------------------------------------------------------------------- */
void ordenar_insercao(Lista *l, Criterio c) {
    No *ordenada = NULL;      /* inicio da lista ja ordenada */
    No *ultimo = NULL;        /* fim da lista ordenada (atalho do melhor caso) */
    No *atual = l->inicio;

    ordenacao_comparacoes = 0;
    while (atual != NULL) {
        No *proximo = atual->prox;   /* guarda antes de mexer em atual->prox */

        if (ordenada == NULL) {
            /* primeiro no: vira a lista ordenada */
            atual->prox = NULL;
            ordenada = ultimo = atual;
        } else if (comparar_servicos(&ultimo->dado, &atual->dado, c) <= 0) {
            /* maior ou igual ao ultimo: encaixa no fim (mantem estabilidade) */
            atual->prox = NULL;
            ultimo->prox = atual;
            ultimo = atual;
        } else if (comparar_servicos(&atual->dado, &ordenada->dado, c) < 0) {
            /* menor que o primeiro: vira o novo inicio */
            atual->prox = ordenada;
            ordenada = atual;
        } else {
            /* procura o ultimo no que e <= atual e encaixa logo depois dele */
            No *p = ordenada;
            while (p->prox != NULL &&
                   comparar_servicos(&p->prox->dado, &atual->dado, c) <= 0) {
                p = p->prox;
            }
            atual->prox = p->prox;
            p->prox = atual;
        }
        atual = proximo;
    }
    l->inicio = ordenada;
    l->fim = ultimo;
}

int lista_esta_ordenada(const Lista *l, Criterio c) {
    const No *atual;
    unsigned long guardado = ordenacao_comparacoes;   /* nao altera a contagem */
    int ok = 1;
    for (atual = l->inicio; atual != NULL && atual->prox != NULL; atual = atual->prox) {
        if (comparar_servicos(&atual->dado, &atual->prox->dado, c) > 0) { ok = 0; break; }
    }
    ordenacao_comparacoes = guardado;
    return ok;
}
