# Roteiro do vídeo de DSA (3 a 4 min)

Gravar a tela com o terminal grande (fonte 16 ou mais) e o VS Code ao lado para mostrar o código. Antes de gravar, na pasta do projeto:

```bash
make clean && make
```

Deixar abertos no editor: `src/lista.c` (função `lista_remover`) e `src/ordenacao.c` (funções `dividir_ao_meio` e `intercalar`).

| Tempo | Quem | Tela | O que fazer e falar | Critério (pontos) |
|---|---|---|---|---|
| 0:00–0:20 | Gabriel | README no GitHub | "Citizen Assist AI em C. Recorte: serviços para pessoas com deficiência em São Paulo. Os 14 exemplos vêm das fichas reais do nosso assistente Mapiii, cada uma com o link oficial." | — |
| 0:20–0:50 | Thor | `src/lista.h` e `src/lista.c` | Mostrar `typedef struct No { Servico dado; struct No *prox; }`, os ponteiros `inicio` e `fim`, o `malloc` em `lista_inserir` e o `free` em `lista_remover`. Explicar os 3 casos da remoção (início, meio, fim). | Lista encadeada e recursos do C (3,0) |
| 0:50–1:30 | Gabriel | terminal: `./citizen` | Opção **9** (carrega 14). Opção **8** (lista). Opção **1**: cadastrar um serviço com Enter no ID (vira 15). Opção **2**, ID 15. Opção **4**: mudar o título. Opção **5**, ID 15, confirmar com `s`. Opção **2**, ID 15 de novo: "Nenhum serviço com esse ID". | CRUD e menu (3,0) |
| 1:30–2:00 | Thor | terminal | Entradas inválidas: opção `abc`, opção `99`, cadastrar com ID `3` (já existe), ID `0`, categoria `9`. Mostrar que cada uma tem uma mensagem e nada quebra. | Validação (3,0) |
| 2:00–2:40 | Gabriel | terminal + `src/ordenacao.c` | Opção **3**, título, `onibus`: acha os 2 com "ônibus" (sem acento). Pesquisa `aeroporto`: "Nenhum serviço encontrado". Opção **6**, título, Merge Sort: lista A–Z e "37 comparações". Mostrar no código o ponteiro lento e o rápido achando o meio, e o `intercalar`. | Busca e ordenação (2,0) |
| 2:40–3:10 | Thor | terminal | Opção **6**, ID, Insertion Sort: "56 comparações". De novo, já ordenada: "13 comparações" (melhor caso O(n)). Opção **7**: total e quantidade por categoria. Opção **10**: endereços dos nós e o último apontando para `(nil)`. Opção **0**: "Memória liberada". | Indicador + demonstração real da estrutura (1,0) |
| 3:10–3:40 | Gabriel | terminal: `make test` + `docs/big-o.md` | Rodar `make test`: 68 testes, 0 falhas, sem vazamento. Mostrar a tabela de comparações: com 4.000 itens, o Merge faz ~43 mil e o Insertion ~4 milhões (O(n log n) × O(n²)). | Big-O e testes (1,0 + 1,0) |
| 3:40–3:55 | Thor | README | Encerrar: "Código, testes, Big-O e prints estão no repositório GS2-DSA." | — |

## Checklist antes de gravar

- [ ] `make` sem avisos e `make test` passando
- [ ] Terminal limpo, fonte grande
- [ ] Os dois sabem explicar a remoção (3 casos) e o Merge Sort (dividir, ordenar as metades, intercalar)
- [ ] Vídeo com no máximo ~4 min
- [ ] Se for para o YouTube: não listado, e colar o link no README
