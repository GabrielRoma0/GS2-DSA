# Documentação técnica

## 1. Recorte escolhido

**Serviços e direitos para pessoas com deficiência em São Paulo.** Famílias de pessoas com deficiência perdem tempo descobrindo *qual* serviço existe, *onde* pedir e *que documentos* levar. O Citizen Assist AI organiza esses serviços num cadastro que pode ser consultado, pesquisado e ordenado.

Os 14 registros de exemplo vêm das fichas do assistente Mapiii, o projeto principal da dupla nesta GS. Cada ficha foi conferida na fonte oficial (SPTrans, INSS/gov.br, Prefeitura de SP, Detran-SP), e o campo `fonte` guarda o link. Nada foi inventado para o trabalho.

Categorias (lista fechada, para a pesquisa e o indicador serem confiáveis):

| Categoria | Exemplos |
|---|---|
| Transporte | Bilhete Único Especial, Atende+, Passe Livre Interestadual |
| Beneficio | BPC |
| Veiculo | isenção de IPI/IOF, Cartão DeFis, renovação da CNH |
| Calcada | conserto de calçada pública, denúncia de calçada particular, obstáculo |
| Atendimento | Central 156, ouvidorias, reclamação de ônibus |

## 2. Modelo do registro

```c
typedef struct {
    int  id;                 /* identificador único, > 0 */
    char titulo[128];        /* obrigatório */
    char categoria[20];      /* uma das 5 categorias */
    char descricao[512];     /* obrigatório */
    char requisitos[768];    /* documentos e condições (opcional) */
    char local[384];         /* onde pedir (obrigatório) */
    char fonte[256];         /* link oficial (opcional) */
} Servico;
```

Os campos são vetores de tamanho fixo: o nó inteiro é alocado com **um** `malloc` e liberado com **um** `free`, o que deixa o gerenciamento de memória simples e sem risco de esquecer de liberar um campo. O custo é espaço: cada nó ocupa 2.080 bytes (`sizeof(No)`, mostrado na opção 10), mesmo quando o texto é curto. Com centenas de serviços isso dá poucas centenas de KB, aceitável para o recorte.

Regras de validação (`servico_validar`):

- `id` inteiro maior que zero e **único** na lista;
- `titulo`, `descricao` e `local` não podem ser vazios (nem só espaços);
- `categoria` precisa ser uma das 5 (no menu a pessoa escolhe pelo número, então não há como digitar uma inválida);
- texto maior que o campo é recusado com aviso, em vez de ser cortado em silêncio.

Toda leitura do teclado usa `fgets` (nunca `scanf("%s")`, que pode escrever além do vetor).

## 3. Lista encadeada

```c
typedef struct No {
    Servico    dado;
    struct No *prox;   /* NULL no último nó */
} No;

typedef struct {
    No *inicio;        /* primeiro nó (NULL se vazia) */
    No *fim;           /* último nó: deixa o encadeamento no fim em O(1) */
    int tamanho;
} Lista;
```

```
 Lista
 ┌────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
 │ inicio ├────►│ id 1 │ prox ─┼────►│ id 2 │ prox ─┼────►│ id 3 │ NULL  │
 │ fim    ├──┐  └──────────────┘     └──────────────┘     └──────────────┘
 │ tam = 3│  └─────────────────────────────────────────────────►▲
 └────────┘
```

| Operação | Como funciona |
|---|---|
| **Inserir** (`lista_inserir`) | valida, procura ID repetido percorrendo a lista, faz `malloc` do nó, copia o servico e liga no fim (`fim->prox = novo; fim = novo`). Se a lista estava vazia, `inicio` também aponta para ele. |
| **Percorrer** (`lista_imprimir`) | `for (p = inicio; p != NULL; p = p->prox)`. |
| **Buscar por ID** | percurso até achar o ID; devolve o nó ou `NULL`. |
| **Pesquisar** | percurso testando se o trecho aparece no título/categoria. Antes de comparar, os dois textos passam por `texto_dobrar`, que deixa tudo minúsculo e troca letras acentuadas do UTF-8 pela letra simples (tabela para o bloco `0xC3`): "Calçada" e "CALCADA" viram "calcada". Mostra todos os resultados ou "Nenhum servico encontrado". |
| **Atualizar** | acha o nó e troca só o conteúdo (`no->dado = copia`). O ID e o ponteiro `prox` não mudam, então a lista continua íntegra. Se o dado novo for inválido, nada muda. |
| **Remover** | percorre com dois ponteiros (`anterior` e `atual`). Três casos: primeiro nó (`inicio = atual->prox`), meio (`anterior->prox = atual->prox`) e último (também `fim = anterior`). Depois, `free(atual)`. |
| **Liberar** | percorre guardando `proximo = atual->prox` **antes** do `free`, porque depois de liberado o nó não pode mais ser lido. Chamado ao sair do programa. |

### Ordenação na própria lista

Os dois algoritmos mexem só nos ponteiros `prox`; os servicos (2 KB cada) nunca são copiados nem levados para um vetor.

- **Merge Sort** (padrão): acha o meio com um ponteiro lento (1 passo) e um rápido (2 passos), corta a lista, ordena cada metade recursivamente e intercala as duas com um nó sentinela. É o algoritmo mais adequado para lista encadeada, porque não precisa de acesso por índice. Estável.
- **Insertion Sort** (para comparar): tira os nós um a um e encaixa cada um na posição certa de uma nova lista. Testa primeiro "vai no fim?" e "vai no início?", e só então percorre. Estável.

Critérios: título (A–Z), categoria (desempate pelo título) ou ID. A comparação de textos também ignora maiúscula e acento, então "Ônibus" fica entre "Central" e "Ouvidoria", e não no fim da lista (o que aconteceria comparando os bytes crus).

## 4. Menu

```
  1 - Cadastrar            6 - Ordenar e listar
  2 - Consultar por ID     7 - Analise (indicadores)
  3 - Pesquisar            8 - Listar todos
  4 - Atualizar            9 - Carregar exemplos
  5 - Excluir             10 - Ver a lista na memoria
  0 - Sair
```

| Situação | O que o programa faz |
|---|---|
| Opção fora de 0–10, letras ou Enter vazio | "Opcao invalida" ou "Entrada invalida" e volta ao menu |
| ID repetido no cadastro | recusa logo após o ID, antes de pedir os outros campos |
| ID zero, negativo ou não numérico | recusa com o motivo |
| Lista vazia em consultar, pesquisar, atualizar, excluir, ordenar, analisar ou listar | "A lista esta vazia. Cadastre ou carregue servicos primeiro." |
| ID inexistente | "Nenhum servico com esse ID." |
| Pesquisa sem resultado | "Nenhum servico encontrado para ..." |
| Texto maior que o campo | "Texto longo demais" e o cadastro é cancelado |
| Exclusão | mostra o servico e pede confirmação (s/n) |
| Fim da entrada (Ctrl+D ou fim do roteiro) | sai normalmente, liberando a memória |

**Análise (opção 7):** total de servicos, quantidade por categoria com uma barra simples e a categoria com mais servicos (com percentual).

**Carga (opção 9):** lê `dados/servicos.txt` (ou o arquivo passado como argumento: `./citizen outro.txt`), uma linha por servico no formato `id|titulo|categoria|descricao|requisitos|local|fonte`. Cada linha passa pela mesma validação do cadastro; linhas inválidas ou com ID repetido são contadas como recusadas e não entram.
