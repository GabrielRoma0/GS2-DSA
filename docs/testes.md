# Evidências de testes

Os testes foram feitos de duas formas, e as duas podem ser repetidas por qualquer pessoa:

1. **Testes automáticos** (`make test`): 68 verificações em [`testes/testes.c`](../testes/testes.c), compiladas com AddressSanitizer + UndefinedBehaviorSanitizer. O sanitizer acusaria vazamento de memória (`malloc` sem `free`), uso de nó já liberado e acesso fora do vetor. Resultado em 17/11/2026: **68 testes, 0 falha(s)**, sem nenhum alerta do sanitizer.
2. **Roteiros no menu** (`make roteiros`): o programa real roda com as entradas de [`testes/roteiros/`](../testes/roteiros/) e a tela completa é gravada em [`testes/saidas/`](../testes/saidas/). Quando a entrada vem de arquivo, o programa repete o que foi "digitado", então a saída mostra pergunta e resposta.

> **Para a entrega:** tirar prints do terminal rodando `make test` e o menu ao vivo (casos abaixo) e colocar em `docs/prints/`.

## 1. Casos normais

Roteiro [`02_crud_normal.txt`](../testes/roteiros/02_crud_normal.txt) → saída [`02_crud_normal.txt`](../testes/saidas/02_crud_normal.txt)

| # | Entrada | Esperado | Observado |
|---|---|---|---|
| N1 | Opção 9 (carregar exemplos) | 14 servicos carregados, 0 recusados | "14 servico(s) carregado(s) … 0 linha(s) recusada(s) … Total: 14." ✅ |
| N2 | Opção 8 (listar) | 14 servicos na ordem do arquivo | IDs 1 a 14 na ordem ✅ |
| N3 | Opção 1, Enter no ID, campos preenchidos, categoria 5 | Cadastra com o ID sugerido 15 | "Servico 15 cadastrado. Total: 15." ✅ |
| N4 | Opção 2, ID 15 | Mostra todos os campos | Ficha completa do ID 15 ✅ |
| N5 | Opção 4, ID 15, novo título, categoria 2, Enter nos outros | Muda título e categoria, mantém o resto e o ID | "Servico 15 atualizado (o ID nao muda)." + "[ 15] Beneficio Rampa no posto de saude (UBS)" ✅ |
| N6 | Opção 5, ID 15, "s" | Exclui | "Servico 15 excluido. Total: 14." ✅ |
| N7 | Opção 2, ID 15 depois de excluir | Não encontra | "Nenhum servico com esse ID. (ID 15)" ✅ |
| N8 | Opção 7 (análise) | Total e contagem por categoria | Total 14; Transporte 3, Beneficio 1, Veiculo 3, Calcada 3, Atendimento 4; maior: Atendimento (29%) ✅ |

## 2. Busca e ordenação

Roteiro [`04_busca_ordenacao.txt`](../testes/roteiros/04_busca_ordenacao.txt) → saída [`04_busca_ordenacao.txt`](../testes/saidas/04_busca_ordenacao.txt)

| # | Entrada | Esperado | Observado |
|---|---|---|---|
| B1 | Pesquisar título "onibus" | Acha os 2 servicos com "ônibus" | IDs 13 e 14 ✅ |
| B2 | Pesquisar título "CALCADA" | Acha os 3 de calçada | IDs 7, 8 e 9 ✅ |
| B3 | Pesquisar categoria "atend" | Os 4 de Atendimento | IDs 10, 11, 13 e 14 ✅ |
| B4 | Pesquisar título "aeroporto" | Nenhum resultado, com aviso | "Nenhum servico encontrado para "aeroporto"." ✅ |
| B5 | Ordenar por título com Merge Sort | A–Z, sem acento atrapalhar | Atende+, Bilhete Único, BPC, Calçada particular, Calçada pública, … Renovar a CNH; 37 comparações ✅ |
| B6 | Ordenar por categoria com Merge Sort | Agrupado por categoria, título A–Z dentro | Atendimento (4), Beneficio, Calcada (3), Transporte (3), Veiculo (3) ✅ |
| B7 | Ordenar por ID com Insertion Sort (lista misturada) | 1 a 14 | 1 a 14, 56 comparações ✅ |
| B8 | Ordenar por ID com Insertion Sort de novo (já ordenada) | Melhor caso: n − 1 comparações | 13 comparações ✅ |
| B9 | Opção 10 | Endereços dos nós, cada `prox` apontando para o próximo e o último para `(nil)` | 14 nós encadeados, último `prox -> (nil)`, 2080 bytes por nó ✅ |

## 3. Lista vazia

Roteiro [`01_lista_vazia.txt`](../testes/roteiros/01_lista_vazia.txt) → saída [`01_lista_vazia.txt`](../testes/saidas/01_lista_vazia.txt)

| # | Entrada (lista vazia) | Esperado | Observado |
|---|---|---|---|
| V1 | Opções 2, 3, 4, 5, 6, 7 e 8 | Aviso de lista vazia em cada uma, sem travar | "A lista esta vazia. Cadastre ou carregue servicos primeiro." nas 7 ✅ |
| V2 | Opção 10 | `inicio` e `fim` nulos, tamanho 0 | "inicio -> (nil) \| fim -> (nil) \| tamanho = 0" ✅ |
| V3 | Opção 0 | Sai liberando a memória | "Memoria liberada (0 nos na lista)." ✅ |

Automáticos (grupo `[lista vazia]`): buscar, remover, atualizar, pesquisar, ordenar com os dois algoritmos, contar e liberar a lista vazia: tudo sem erro ✅.

## 4. Entradas inválidas e registros inexistentes

Roteiro [`03_entradas_invalidas.txt`](../testes/roteiros/03_entradas_invalidas.txt) → saída [`03_entradas_invalidas.txt`](../testes/saidas/03_entradas_invalidas.txt)

| # | Entrada | Esperado | Observado |
|---|---|---|---|
| I1 | Menu: "abc" | Recusa | "Entrada invalida: digite um numero inteiro." ✅ |
| I2 | Menu: 99 e −1 | Recusa | "Opcao invalida: escolha de 0 a 10." ✅ |
| I3 | Menu: Enter vazio | Recusa | "Entrada invalida…" ✅ |
| I4 | Cadastrar com ID 3 (já existe) | Recusa logo no ID | "Ja existe um servico com esse ID." ✅ |
| I5 | Cadastrar com ID 0 e −7 | Recusa | "ID invalido: use um numero inteiro maior que zero." ✅ |
| I6 | Cadastrar com ID "abc" | Recusa | "Entrada invalida…" ✅ |
| I7 | Categoria 9 | Recusa | "Categoria invalida: escolha de 1 a 5." ✅ |
| I8 | Título vazio | Recusa e não cadastra | "Titulo, descricao e local sao obrigatorios." ✅ |
| I9 | Título com 200 caracteres | Recusa sem estourar o vetor | "Texto longo demais (maximo de 126 caracteres)." ✅ |
| I10 | Atualizar ID 999 | Não encontrado | "Nenhum servico com esse ID. (ID 999)" ✅ |
| I11 | Excluir ID 999 | Não encontrado | "Nenhum servico com esse ID. (ID 999)" ✅ |
| I12 | Excluir ID 3 e responder "n" | Cancela, lista intacta | "Exclusao cancelada." e o total continua 14 ✅ |
| I13 | Consultar ID "xyz" | Recusa | "Entrada invalida…" ✅ |
| I14 | Pesquisar por opção 3 | Recusa | "Opcao invalida: escolha de 1 a 2." ✅ |
| I15 | Pesquisar texto vazio | Pede texto | "Digite algum texto para pesquisar." ✅ |
| I16 | Ordenar por critério 4 / algoritmo 9 | Recusa | "Opcao invalida: escolha de 1 a 3." / "…de 1 a 2." ✅ |
| I17 | Listar no fim | Nada foi alterado pelos erros | "Total: 14." ✅ |

## 5. Testes automáticos (resumo por grupo)

| Grupo | O que confere |
|---|---|
| lista vazia | todas as operações com a lista vazia |
| inserção e validação | ordem de cadastro, ID duplicado/zero/negativo, campo vazio, categoria inválida, sugestão de ID, liberar |
| busca | primeiro, último, inexistente, pesquisa sem acento/maiúscula, sem resultado, texto vazio |
| remoção | início, meio, fim (o ponteiro `fim` é atualizado), inexistente, remover tudo |
| atualização | ID não muda, ponteiros não mudam, dado inválido não altera o registro |
| ordenação | Merge e Insertion por título, categoria e ID; 1 nó; **estabilidade** |
| complexidade | n = 100, 1.000 e 4.000: as duas ordenam certo e o Insertion faz mais de 10× as comparações do Merge (ver [big-o.md](big-o.md)) |
| carga do arquivo | 14 servicos, recarga recusa os 14 repetidos, arquivo inexistente |
| leitura de linha | campo vazio permitido, número errado de campos, ID não numérico |

Em todo teste que altera a lista, a função `integra()` confere se o número de nós percorridos é igual a `tamanho`, se o último nó é o `fim` e se a lista vazia tem `inicio == NULL`.
