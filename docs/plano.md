# GS 2026.2 · Data Structures and Algorithms (DSA) · Plano de trabalho

Contribuição da disciplina ao Mapiii (Citizen Assist AI). Escrito em 17/11/2026 a partir do enunciado `GS_Citizen_Assist_AI_1CC_FEV(4).pdf` (turma 1CC-FEV, vale 10,0).

> **Regra desta pasta:** nada daqui vai para o repositório oficial (`mapiii-assistente` / `GS-2SEM-2026`) e nada é mexido lá. O Mapiii serve só de **base** (recorte, público, tom, fichas com fonte). A entrega de DSA é uma pasta/repositório **separado**.

---

> **Status 17/11/2026:** código, testes (68 automáticos + 4 roteiros), README, documentação, Big-O e evidências prontos. **Falta da dupla:** ler o código para saber explicar, rodar `make test` e o menu e tirar prints (`docs/prints/`), criar o repositório público no GitHub e dar push, gravar/apresentar a demonstração (passo 7) e entregar no Portal.

## 0. O que o professor vai cobrar (resumo do enunciado)

| Pontos | Critério | Em uma frase |
|---|---|---|
| **3,0** | Funcionamento, CRUD, menu e validação | Menu com cadastro, consulta, pesquisa, atualização, exclusão, ordenação, análise e sair; trata opção inválida, id duplicado e lista vazia. |
| **3,0** | Lista encadeada e recursos da linguagem | Lista **própria** (nó com ponteiro/referência), com inserção, percurso e remoção. Em C: `struct`, `typedef`, ponteiros, `malloc`/`free`. |
| **2,0** | Busca e ordenação implementadas | Busca por id + por pelo menos um atributo (explicada); um sort estudado (Selection, Insertion, Merge ou Quick) **escrito à mão**. `qsort`/`sorted()` sozinhos não contam. |
| **1,0** | Organização, documentação e Big-O | Big-O de inserção, busca, remoção e ordenação **de acordo com o código real**, com justificativa. |
| **1,0** | Testes e demonstração | Tabela de testes (entrada, esperado, observado) com casos normais, inexistentes, inválidos e lista vazia + demonstração ao vivo/vídeo. |

Entregáveis: código-fonte com instruções de compilação/execução · documentação técnica (recorte, modelo do registro, lista, menu) · análise Big-O · evidências de testes · demonstração.

Opcionais (só depois do obrigatório): persistência em arquivo, filtros, relatórios, outras estruturas.


## Cronograma (entrega 18/11)

| Quando | O quê |
|---|---|
| 17/11 noite | Passos 2 a 5: Claude escreve lista, menu, busca, ordenação e testes; dupla compila e roda |
| 18/11 manhã | Passo 6: docs e Big-O; dupla revisa e roda os roteiros para os prints |
| 18/11 tarde | Passo 7 e 8: demonstração, push no GitHub, entrega no Portal |

---

## 1. A ideia (recomendação)

**Recorte:** "Direitos e serviços para pessoas com deficiência em São Paulo", o mesmo do Mapiii. Cada registro é um **serviço** (Bilhete Único Especial, BPC, Atende+, Passe Livre, Cartão DeFis, Central 156…).

**Por que esse recorte:** a dupla já tem 14 fichas reais e conferidas em `GS-2SEM-2026/data/kb.json`, então não precisa inventar conteúdo. Os dados de exemplo vêm de lá (título, resumo, onde, fonte), copiados para um arquivo de carga desta pasta.

**Modelo do registro (`Servico`):**

| Campo | Tipo (C) | Exemplo |
|---|---|---|
| `id` | `int` (único, > 0) | 3 |
| `titulo` | `char[100]` | "Atende+: transporte porta a porta" |
| `categoria` | `char[30]` (lista fechada) | Transporte · Benefício · Documentação · Calçada · Atendimento/Ouvidoria |
| `descricao` | `char[300]` | resumo da ficha |
| `requisitos` | `char[300]` | condições/documentos |
| `local` | `char[150]` | onde pedir (site, 156, posto) |
| `fonte` | `char[200]` | link oficial da ficha |

**Lista encadeada:** simples (`struct No { Servico dado; struct No *prox; }`), com ponteiro para `inicio` e um contador `tamanho`. Inserção no **fim** (mantém a ordem de cadastro) e remoção por id com os três casos (início, meio, fim).

**Busca:**
- por id: busca sequencial na lista → O(n);
- por título/categoria: busca sequencial por **trecho** sem diferenciar maiúscula/minúscula, mostrando todos os resultados e "Nenhum serviço encontrado" quando vazio → O(n·m).

**Ordenação (recomendação: Merge Sort na própria lista):** é o algoritmo natural para lista encadeada (não precisa de acesso por índice), é estável e O(n log n) sempre. Ordena por título (A–Z) ou por categoria. Como reserva didática, um **Insertion Sort** por id dá para comparar O(n²) × O(n log n) na análise.

**Análise (indicadores):** total de serviços e quantidade por categoria.

**Linguagem:** recomendação **C** (o trabalho da GS anterior de DSA da dupla foi em C, `~/GS-DSA/GS-DSA.c`, e o critério de 3,0 pontos premia ponteiro/`malloc`/`free`). Só bibliotecas padrão (`stdio`, `stdlib`, `string`, `ctype`).

---

## 2. Como organizar a pasta

```
DSA - GS2 - 2026/
├── GS_Citizen_Assist_AI_1CC_FEV(4).pdf   enunciado
├── README.md                              integrantes, recorte, como compilar e rodar
├── src/
│   ├── servico.h / servico.c              struct Servico, leitura e validação de campos
│   ├── lista.h / lista.c                  lista encadeada: inserir, percorrer, buscar, remover, liberar
│   ├── ordenacao.h / ordenacao.c          merge sort (e insertion sort) na lista
│   └── main.c                             menu
├── dados/servicos.txt                     carga inicial (fichas do Mapiii) — opcional
├── testes/
│   ├── testes.c                           testes automáticos com assert
│   └── roteiros/*.txt                     entradas para rodar o menu com `< arquivo`
├── docs/
│   ├── plano.md                           este arquivo
│   ├── documentacao.md                    recorte, modelo, lista, menu (entregável)
│   ├── big-o.md                           análise de algoritmos (entregável)
│   └── testes.md                          tabela de evidências + prints (entregável)
└── Makefile                               `make`, `make test`, `make valgrind`
```

---

## 3. Passo a passo (em ordem)

Legenda: **C** = Claude escreve, **D** = dupla faz/decide.

### Passo 1 · Decidir (D, 10 min)
Confirmar linguagem, recorte e prazo (ver seção 6).

### Passo 2 · Lista encadeada + CRUD (C)
`lista.c` com `criar`, `inserir_fim`, `buscar_id`, `remover_id`, `percorrer/imprimir`, `liberar`. Validação: id duplicado recusado, id ≤ 0 recusado, campos vazios recusados, lista vazia avisada em todas as operações. Leitura segura com `fgets` (nada de `scanf("%s")` que estoura buffer).

### Passo 3 · Menu (C)
```
1 Cadastrar  2 Consultar por id  3 Pesquisar (título/categoria)  4 Atualizar
5 Excluir    6 Ordenar e listar  7 Análise (total e por categoria)
8 Carregar exemplos (opcional)   0 Sair
```
Atualização mantém o id (ou valida o novo contra duplicado) e não quebra os ponteiros.

### Passo 4 · Busca e ordenação (C)
Busca por trecho; Merge Sort (dividir com ponteiro lento/rápido, intercalar) e Insertion Sort por id. Cada função com comentário explicando o algoritmo (o enunciado pede "implementar **e explicar**").

### Passo 5 · Testes (C escreve, D roda e confere)
- `testes.c` com `assert` cobrindo: inserir, duplicado, remover início/meio/fim, remover inexistente, buscar sem resultado, ordenar lista vazia/1 item/vários.
- Roteiros de menu (`./citizen < testes/roteiros/lista_vazia.txt`) para gerar as evidências.
- `valgrind --leak-check=full` sem vazamento (prova o `free`; ótimo para os 3,0 da lista).

### Passo 6 · Documentação e Big-O (C escreve, D revisa e entende)
`documentacao.md`, `big-o.md` (tabela + justificativa linha a linha) e `testes.md` (tabela entrada/esperado/observado com prints).

### Passo 7 · Demonstração (D)
Rodar ao vivo ou gravar: carregar exemplos → pesquisar → ordenar (mostrar antes/depois) → excluir → casos inválidos → análise → sair com memória liberada. O enunciado pede "funcionamento real das estruturas", então vale mostrar o percurso da lista (ex.: imprimir os endereços `prox` numa opção de depuração).

### Passo 8 · Entrega (D)
Conferir no Portal o formato (ZIP? link de repo? `.txt`?), integrantes e RM no README.

---

## 4. Big-O esperado (rascunho, confirmar contra o código final)

| Operação | Complexidade | Por quê |
|---|---|---|
| Inserir no fim | O(n) | percorre até o último (O(1) se guardarmos ponteiro `fim`, e aí documentar isso) |
| Verificar id duplicado | O(n) | busca sequencial antes de inserir |
| Buscar por id | O(n) | lista não tem acesso por índice |
| Pesquisar por trecho | O(n·m) | n nós × tamanho do texto |
| Remover por id | O(n) | achar o anterior; religar ponteiros é O(1) |
| Merge Sort | O(n log n) tempo, O(log n) pilha | divide ao meio, intercala em O(n) |
| Insertion Sort | O(n²) pior, O(n) já ordenada | comparação didática |

---

## 5. Checklist final

- [ ] Compila sem warnings com `gcc -Wall -Wextra -std=c11`
- [ ] Nenhuma função pronta de ordenação/busca usada como solução
- [ ] Opção inválida, id duplicado, lista vazia e registro inexistente tratados
- [ ] `valgrind` sem vazamento
- [ ] Big-O bate com o código entregue
- [ ] Tabela de testes com esperado × observado e prints
- [ ] README com integrantes, RM, como compilar e rodar
- [ ] Nenhum conteúdo de serviço inventado (dados vêm das fichas do Mapiii)

---

## 6. Decisões da dupla (preencher)

| Pergunta | Recomendação | Decisão |
|---|---|---|
| Linguagem | C | **C** (17/11) |
| Recorte | Serviços para PcD em SP (fichas do Mapiii) | |
| Ordenação principal | Merge Sort | |
| Persistência em arquivo | Só se sobrar tempo | |
| Prazo e formato de entrega | conferir no Portal | **18/11** (formato: conferir no Portal) |
| Repositório no GitHub | público | **Sim, público** (dupla cria o repo e dá push) |
