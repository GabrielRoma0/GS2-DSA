/*
 * servico.h - o registro do Citizen Assist AI (recorte: servicos para
 * pessoas com deficiencia em Sao Paulo) e as funcoes de texto usadas pela
 * busca e pela ordenacao.
 */
#ifndef SERVICO_H
#define SERVICO_H

#include <stddef.h>

/* Tamanhos maximos de cada campo (contando o '\0' do fim). */
#define TAM_TITULO     128
#define TAM_CATEGORIA   20
#define TAM_DESCRICAO  512
#define TAM_REQUISITOS 768
#define TAM_LOCAL      384
#define TAM_FONTE      256

/* Um servico publico util ao cidadao. */
typedef struct {
    int  id;                          /* identificador unico, maior que zero */
    char titulo[TAM_TITULO];
    char categoria[TAM_CATEGORIA];    /* uma de CATEGORIAS[] */
    char descricao[TAM_DESCRICAO];
    char requisitos[TAM_REQUISITOS];  /* documentos e condicoes */
    char local[TAM_LOCAL];            /* onde pedir */
    char fonte[TAM_FONTE];            /* link oficial de onde a informacao veio */
} Servico;

/* Resultado das operacoes (usado pela lista e pelo menu). */
typedef enum {
    OK = 0,
    ERRO_ID_INVALIDO,
    ERRO_ID_DUPLICADO,
    ERRO_CAMPO_VAZIO,
    ERRO_CATEGORIA,
    ERRO_NAO_ENCONTRADO,
    ERRO_LISTA_VAZIA,
    ERRO_MEMORIA
} Resultado;

/* Lista fechada de categorias do recorte. */
#define TOTAL_CATEGORIAS 5
extern const char *const CATEGORIAS[TOTAL_CATEGORIAS];

/* Devolve o indice da categoria em CATEGORIAS[] ou -1 se nao existir
 * (compara sem diferenciar maiuscula, minuscula e acento). */
int categoria_indice(const char *nome);

/* Confere id, campos obrigatorios e categoria. Nao olha a lista. */
Resultado servico_validar(const Servico *s);

/* Le uma linha "id|titulo|categoria|descricao|requisitos|local|fonte"
 * do arquivo de carga. Devolve 1 se a linha foi lida, 0 se esta mal formada. */
int servico_de_linha(const char *linha, Servico *s);

/* Mostra o servico: resumido (uma linha) ou detalhado (todos os campos). */
void servico_imprimir(const Servico *s, int detalhado);

/* Mensagem em portugues para cada Resultado. */
const char *resultado_texto(Resultado r);

/* --- Texto sem diferenciar maiuscula/minuscula e acento ---------------
 * "Ônibus", "ONIBUS" e "onibus" sao tratados como iguais. Funciona com
 * texto em UTF-8 (letras acentuadas do portugues). */

/* Escreve em 'saida' a versao "dobrada" (minuscula e sem acento) de 'texto'. */
void texto_dobrar(const char *texto, char *saida, size_t tam_saida);

/* 1 se 'trecho' aparece dentro de 'texto', 0 se nao. Trecho vazio nao casa. */
int texto_contem(const char *texto, const char *trecho);

/* Compara dois textos como strcmp, mas ignorando maiuscula e acento. */
int texto_comparar(const char *a, const char *b);

/* Copia com limite e sempre termina com '\0' (strncpy nao garante). */
void copiar_texto(char *destino, const char *origem, size_t tam_destino);

#endif
