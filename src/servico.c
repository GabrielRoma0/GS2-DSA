/*
 * servico.c - validacao, leitura do arquivo de carga, impressao e
 * funcoes de texto (busca sem acento) do registro Servico.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "servico.h"

const char *const CATEGORIAS[TOTAL_CATEGORIAS] = {
    "Transporte", "Beneficio", "Veiculo", "Calcada", "Atendimento"
};

/*
 * Tabela para tirar acento de letras UTF-8 que comecam com o byte 0xC3
 * (o bloco Latin-1 do portugues: á, ã, ç, é, ô, ú ...). O segundo byte vai
 * de 0x80 a 0xBF; a posicao (byte - 0x80) diz qual letra simples usar.
 * Ex.: "ô" = C3 B4 -> posicao 0x34 -> 'o'.
 */
static const char SEM_ACENTO[64] =
    "aaaaaaaceeeeiiiidnoooooxouuuuyps"   /* 0x80-0x9F: maiusculas */
    "aaaaaaaceeeeiiiidnoooooxouuuuypy";  /* 0xA0-0xBF: minusculas */

void texto_dobrar(const char *texto, char *saida, size_t tam_saida) {
    size_t j = 0;
    const unsigned char *p = (const unsigned char *)texto;
    if (tam_saida == 0) return;
    while (*p != '\0' && j + 1 < tam_saida) {
        if (p[0] == 0xC3 && p[1] >= 0x80 && p[1] <= 0xBF) {
            saida[j++] = SEM_ACENTO[p[1] - 0x80];
            p += 2;
        } else {
            saida[j++] = (char)tolower(*p);
            p += 1;
        }
    }
    saida[j] = '\0';
}

/* Maior campo possivel; usado para os buffers das funcoes abaixo. */
#define TAM_DOBRA TAM_REQUISITOS

int texto_contem(const char *texto, const char *trecho) {
    char t[TAM_DOBRA], b[TAM_DOBRA];
    if (trecho == NULL || trecho[0] == '\0') return 0;
    texto_dobrar(texto, t, sizeof t);
    texto_dobrar(trecho, b, sizeof b);
    return strstr(t, b) != NULL;
}

int texto_comparar(const char *a, const char *b) {
    char x[TAM_DOBRA], y[TAM_DOBRA];
    texto_dobrar(a, x, sizeof x);
    texto_dobrar(b, y, sizeof y);
    return strcmp(x, y);
}

void copiar_texto(char *destino, const char *origem, size_t tam_destino) {
    if (tam_destino == 0) return;
    strncpy(destino, origem, tam_destino - 1);
    destino[tam_destino - 1] = '\0';
}

int categoria_indice(const char *nome) {
    for (int i = 0; i < TOTAL_CATEGORIAS; i++) {
        if (texto_comparar(nome, CATEGORIAS[i]) == 0) return i;
    }
    return -1;
}

/* 1 se o texto e vazio ou so tem espacos. */
static int vazio(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

Resultado servico_validar(const Servico *s) {
    if (s->id <= 0) return ERRO_ID_INVALIDO;
    if (vazio(s->titulo) || vazio(s->descricao) || vazio(s->local)) return ERRO_CAMPO_VAZIO;
    if (categoria_indice(s->categoria) < 0) return ERRO_CATEGORIA;
    return OK;
}

int servico_de_linha(const char *linha, Servico *s) {
    char copia[TAM_TITULO + TAM_CATEGORIA + TAM_DESCRICAO + TAM_REQUISITOS + TAM_LOCAL + TAM_FONTE + 32];
    char *campos[7];
    int n = 0;
    char *p, *fim;
    long id;

    copiar_texto(copia, linha, sizeof copia);
    copia[strcspn(copia, "\r\n")] = '\0';

    /* Separa pelos '|' sem usar strtok, para aceitar campos vazios. */
    p = copia;
    campos[n++] = p;
    while (*p != '\0' && n < 7) {
        if (*p == '|') {
            *p = '\0';
            campos[n++] = p + 1;
        }
        p++;
    }
    if (n != 7 || strchr(campos[6], '|') != NULL) return 0;

    id = strtol(campos[0], &fim, 10);
    if (fim == campos[0] || *fim != '\0' || id <= 0 || id > 999999) return 0;

    memset(s, 0, sizeof *s);
    s->id = (int)id;
    copiar_texto(s->titulo,     campos[1], sizeof s->titulo);
    copiar_texto(s->categoria,  campos[2], sizeof s->categoria);
    copiar_texto(s->descricao,  campos[3], sizeof s->descricao);
    copiar_texto(s->requisitos, campos[4], sizeof s->requisitos);
    copiar_texto(s->local,      campos[5], sizeof s->local);
    copiar_texto(s->fonte,      campos[6], sizeof s->fonte);
    return 1;
}

void servico_imprimir(const Servico *s, int detalhado) {
    if (!detalhado) {
        printf("  [%3d] %-12s %s\n", s->id, s->categoria, s->titulo);
        return;
    }
    printf("  ------------------------------------------------------------\n");
    printf("  ID .........: %d\n", s->id);
    printf("  Titulo .....: %s\n", s->titulo);
    printf("  Categoria ..: %s\n", s->categoria);
    printf("  Descricao ..: %s\n", s->descricao);
    printf("  Requisitos .: %s\n", s->requisitos[0] ? s->requisitos : "(nao informado)");
    printf("  Onde pedir .: %s\n", s->local);
    printf("  Fonte ......: %s\n", s->fonte[0] ? s->fonte : "(nao informada)");
    printf("  ------------------------------------------------------------\n");
}

const char *resultado_texto(Resultado r) {
    switch (r) {
        case OK:                  return "Operacao concluida.";
        case ERRO_ID_INVALIDO:    return "ID invalido: use um numero inteiro maior que zero.";
        case ERRO_ID_DUPLICADO:   return "Ja existe um servico com esse ID.";
        case ERRO_CAMPO_VAZIO:    return "Titulo, descricao e local sao obrigatorios.";
        case ERRO_CATEGORIA:      return "Categoria invalida.";
        case ERRO_NAO_ENCONTRADO: return "Nenhum servico com esse ID.";
        case ERRO_LISTA_VAZIA:    return "A lista esta vazia. Cadastre ou carregue servicos primeiro.";
        case ERRO_MEMORIA:        return "Sem memoria para cadastrar.";
    }
    return "Erro desconhecido.";
}
