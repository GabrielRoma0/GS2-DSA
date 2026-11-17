# Citizen Assist AI · DSA (GS 2026.2)

Protótipo em **C** do Citizen Assist AI para a disciplina **Data Structures and Algorithms** (turma 1CC-FEV, Global Solution 2026.2 · CitizenTech).

**Integrantes**

| Nome | RM |
|---|---|
| Gabriel Botelho Romão | 570589 |
| Thor Ferreira Camargo | 569543 |

## Recorte

**Serviços e direitos para pessoas com deficiência na cidade de São Paulo** (transporte, benefícios, veículo, calçadas e canais de atendimento). É o mesmo público do nosso projeto principal da GS, o assistente **Mapiii**: os 14 serviços de exemplo em [`dados/servicos.txt`](dados/servicos.txt) foram tirados das fichas que a dupla conferiu nas fontes oficiais, e cada um traz o link de onde a informação veio.

> Protótipo de estudo, não é canal oficial. Confirme sempre na fonte indicada.

## O que o sistema faz

| Requisito do enunciado | Onde está |
|---|---|
| Lista encadeada própria (inserção, percurso, remoção) com `struct`, `typedef`, ponteiros, `malloc` e `free` | [`src/lista.c`](src/lista.c) |
| Cadastro com ID único e campos pertinentes | `lista_inserir` + opção 1 do menu |
| Consulta por ID e pesquisa por título ou categoria, tratando "sem resultado" | `lista_buscar_id`, `lista_pesquisar` + opções 2 e 3 |
| Atualização e exclusão preservando a integridade da lista | `lista_atualizar`, `lista_remover` + opções 4 e 5 |
| Ordenação com algoritmo implementado pela equipe | **Merge Sort** e **Insertion Sort** direto nos ponteiros, em [`src/ordenacao.c`](src/ordenacao.c) (sem `qsort`) |
| Indicador útil | total e quantidade por categoria (opção 7) |
| Menu com tratamento de opção inválida, ID duplicado e lista vazia | [`src/main.c`](src/main.c) |
| Análise Big-O | [`docs/big-o.md`](docs/big-o.md) |
| Evidências de testes | [`docs/testes.md`](docs/testes.md), [`testes/`](testes/) |

Extras: pesquisa sem diferenciar maiúscula e acento ("onibus" acha "ônibus"), carga dos exemplos por arquivo (opção 9) e a opção 10, que mostra a lista **na memória** (endereço de cada nó e do próximo), para a demonstração.

## Como compilar e rodar

Precisa só do `gcc` (bibliotecas padrão do C). Rode **de dentro desta pasta**, para o programa achar `dados/servicos.txt`.

**Linux / macOS / WSL**

```bash
make
```

```bash
./citizen
```

**Windows (MinGW / gcc no PowerShell ou cmd)**

```bash
gcc -std=c11 -Wall -Wextra -o citizen.exe src/servico.c src/lista.c src/ordenacao.c src/main.c
```

```bash
citizen.exe
```

Dentro do menu, use a opção **9** para carregar os 14 serviços de exemplo.

## Testes

```bash
make test
```

Roda 68 verificações automáticas ([`testes/testes.c`](testes/testes.c)), compiladas com AddressSanitizer: se algum `malloc` ficasse sem `free`, ou se um nó fosse usado depois de liberado, o teste falharia.

```bash
make roteiros
```

Roda o menu com as entradas de [`testes/roteiros/`](testes/roteiros/) (lista vazia, CRUD normal, entradas inválidas, busca e ordenação) e grava o que apareceu na tela em [`testes/saidas/`](testes/saidas/).

## Estrutura

```
src/servico.h/.c     registro Servico, validação, texto sem acento
src/lista.h/.c       lista encadeada
src/ordenacao.h/.c   Merge Sort e Insertion Sort na lista
src/main.c           menu
dados/servicos.txt   carga de exemplo (14 serviços com fonte)
testes/              testes automáticos, roteiros e saídas
docs/                documentação técnica, Big-O, testes e plano
```

## Documentação

- [Documentação técnica](docs/documentacao.md): recorte, modelo do registro, lista encadeada e menu
- [Análise Big-O](docs/big-o.md)
- [Evidências de testes](docs/testes.md) · [prints](docs/prints/)
