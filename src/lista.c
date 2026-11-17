/*
 * lista.c - operacoes da lista encadeada de servicos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void lista_iniciar(Lista *l) {
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

No *lista_buscar_id(const Lista *l, int id) {
    /* Busca sequencial: a lista nao tem acesso por indice, entao o unico
     * jeito e seguir os ponteiros 'prox' do primeiro ao ultimo no. */
    No *atual = l->inicio;
    while (atual != NULL) {
        if (atual->dado.id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

Resultado lista_inserir(Lista *l, const Servico *s) {
    Resultado r = servico_validar(s);
    No *novo;

    if (r != OK) return r;
    if (lista_buscar_id(l, s->id) != NULL) return ERRO_ID_DUPLICADO;

    novo = (No *)malloc(sizeof(No));
    if (novo == NULL) return ERRO_MEMORIA;
    novo->dado = *s;          /* copia a struct inteira para o no */
    novo->prox = NULL;

    if (l->inicio == NULL) {  /* lista vazia: o novo no e o primeiro e o ultimo */
        l->inicio = novo;
    } else {                  /* senao: o antigo ultimo aponta para o novo */
        l->fim->prox = novo;
    }
    l->fim = novo;
    l->tamanho++;
    return OK;
}

int lista_pesquisar(const Lista *l, Campo campo, const char *trecho,
                    const No *achados[], int max) {
    int total = 0;
    const No *atual;
    for (atual = l->inicio; atual != NULL; atual = atual->prox) {
        const char *texto = (campo == CAMPO_TITULO) ? atual->dado.titulo
                                                    : atual->dado.categoria;
        if (texto_contem(texto, trecho)) {
            if (total < max) achados[total] = atual;
            total++;
        }
    }
    return total;
}

Resultado lista_atualizar(Lista *l, int id, const Servico *novo) {
    No *no;
    Servico copia;
    Resultado r;

    if (l->inicio == NULL) return ERRO_LISTA_VAZIA;
    no = lista_buscar_id(l, id);
    if (no == NULL) return ERRO_NAO_ENCONTRADO;

    copia = *novo;
    copia.id = id;            /* o ID nao muda: preserva a identidade do registro */
    r = servico_validar(&copia);
    if (r != OK) return r;    /* dado invalido: o registro antigo fica intacto */

    no->dado = copia;         /* so o conteudo muda; 'prox' continua o mesmo */
    return OK;
}

Resultado lista_remover(Lista *l, int id) {
    No *anterior = NULL;
    No *atual = l->inicio;

    if (l->inicio == NULL) return ERRO_LISTA_VAZIA;

    /* Anda com dois ponteiros: precisamos do no ANTERIOR para religar. */
    while (atual != NULL && atual->dado.id != id) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) return ERRO_NAO_ENCONTRADO;

    if (anterior == NULL) {
        l->inicio = atual->prox;      /* caso 1: era o primeiro */
    } else {
        anterior->prox = atual->prox; /* caso 2 e 3: meio ou fim, "pula" o no */
    }
    if (atual == l->fim) {
        l->fim = anterior;            /* caso 3: era o ultimo; NULL se a lista esvaziou */
    }
    free(atual);
    l->tamanho--;
    return OK;
}

void lista_imprimir(const Lista *l, int detalhado) {
    const No *atual;
    for (atual = l->inicio; atual != NULL; atual = atual->prox) {
        servico_imprimir(&atual->dado, detalhado);
    }
}

void lista_imprimir_estrutura(const Lista *l) {
    const No *atual;
    int i = 0;
    printf("  inicio -> %p | fim -> %p | tamanho = %d | %lu bytes por no\n",
           (void *)l->inicio, (void *)l->fim, l->tamanho, (unsigned long)sizeof(No));
    for (atual = l->inicio; atual != NULL; atual = atual->prox, i++) {
        printf("  no %2d em %p  [id %3d]  prox -> %p\n",
               i, (const void *)atual, atual->dado.id, (void *)atual->prox);
    }
    printf("  (prox = (nil)/0 marca o fim da lista)\n");
}

void lista_contar_categorias(const Lista *l, int contagem[TOTAL_CATEGORIAS]) {
    const No *atual;
    for (int i = 0; i < TOTAL_CATEGORIAS; i++) contagem[i] = 0;
    for (atual = l->inicio; atual != NULL; atual = atual->prox) {
        int c = categoria_indice(atual->dado.categoria);
        if (c >= 0) contagem[c]++;
    }
}

int lista_proximo_id(const Lista *l) {
    int maior = 0;
    const No *atual;
    for (atual = l->inicio; atual != NULL; atual = atual->prox) {
        if (atual->dado.id > maior) maior = atual->dado.id;
    }
    return maior + 1;
}

int lista_carregar_arquivo(Lista *l, const char *caminho, int *recusadas) {
    FILE *arq = fopen(caminho, "r");
    char linha[4096];
    int inseridos = 0;
    Servico s;

    *recusadas = 0;
    if (arq == NULL) return -1;

    while (fgets(linha, sizeof linha, arq) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') continue;
        if (servico_de_linha(linha, &s) && lista_inserir(l, &s) == OK) {
            inseridos++;
        } else {
            (*recusadas)++;
        }
    }
    fclose(arq);
    return inseridos;
}

void lista_liberar(Lista *l) {
    No *atual = l->inicio;
    while (atual != NULL) {
        No *proximo = atual->prox;  /* guarda antes de liberar: depois do free
                                       nao se pode mais ler 'atual->prox' */
        free(atual);
        atual = proximo;
    }
    lista_iniciar(l);
}
