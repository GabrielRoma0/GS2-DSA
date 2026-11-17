# Análise de algoritmos (Big-O)

Notação: **n** = quantidade de servicos na lista; **m** = tamanho dos textos comparados (no máximo 767 caracteres, então na prática é uma constante). A análise segue o código entregue, não a versão "de livro" de cada estrutura.

## Resumo

| Operação | Função | Tempo | Memória extra |
|---|---|---|---|
| Inserir (cadastrar) | `lista_inserir` | **O(n)** | O(1) (um nó) |
| Buscar por ID | `lista_buscar_id` | **O(n)** | O(1) |
| Pesquisar por título/categoria | `lista_pesquisar` | **O(n · m)** | O(m) |
| Atualizar | `lista_atualizar` | **O(n)** | O(1) |
| Remover | `lista_remover` | **O(n)** | O(1) |
| Percorrer / listar | `lista_imprimir` | O(n) | O(1) |
| Contar por categoria | `lista_contar_categorias` | O(n) | O(1) |
| Liberar tudo | `lista_liberar` | O(n) | O(1) |
| **Merge Sort** | `ordenar_merge` | **O(n log n)** em todos os casos | O(log n) (pilha da recursão) |
| **Insertion Sort** | `ordenar_insercao` | **O(n²)** pior caso e caso médio; **O(n)** melhor caso | O(1) |

## Justificativas

### Inserção: O(n)

Ligar o nó novo no fim custa **O(1)**, porque a lista guarda o ponteiro `fim` (sem ele seria preciso andar até o último nó). Mas antes de inserir, `lista_inserir` chama `lista_buscar_id` para recusar ID repetido, e essa busca percorre a lista. A inserção completa fica então **O(n)**, dominada pela checagem de duplicado, que é obrigatória pelo enunciado.

### Busca por ID: O(n)

Lista encadeada não tem acesso por índice: para chegar ao k-ésimo nó é preciso seguir k ponteiros. A busca é **sequencial**: melhor caso O(1) (o ID está no primeiro nó), pior caso O(n) (está no último ou não existe), caso médio n/2 comparações, ou seja, O(n). Busca binária não ajudaria aqui: mesmo com a lista ordenada, chegar ao meio custa O(n).

### Pesquisa por trecho: O(n · m)

Para cada um dos n nós, o texto é "dobrado" (minúsculas e sem acento, O(m)) e procurado com `strstr`. O pior caso do `strstr` é O(m · k) (k = tamanho do trecho), mas com textos curtos isso se comporta como O(m). Total **O(n · m)**; como m é limitado pelo tamanho do campo, na prática cresce linearmente com n. A pesquisa sempre percorre a lista inteira, porque pode haver vários resultados.

### Atualização: O(n)

Achar o nó é a busca por ID, O(n). Trocar o conteúdo é uma cópia de struct de tamanho fixo, O(1). Os ponteiros não mudam.

### Remoção: O(n)

Achar o nó (e o anterior a ele) é O(n). Religar os ponteiros (`anterior->prox = atual->prox`) e o `free` são O(1). Melhor caso: remover o primeiro nó, O(1).

### Merge Sort: O(n log n)

- `dividir_ao_meio` anda pela metade da lista com os ponteiros lento/rápido: O(n) por nível.
- `intercalar` visita cada nó das duas metades uma vez: O(n) por nível.
- A lista é dividida ao meio até sobrar 1 nó: são ⌈log₂ n⌉ níveis.
- Total: O(n) por nível × log n níveis = **O(n log n)**, em qualquer ordem de entrada (melhor, médio e pior caso iguais).
- Memória: como só os ponteiros são trocados, não há vetor auxiliar (num vetor, o Merge Sort precisa de O(n) extra). Fica só a pilha da recursão, com profundidade log n: **O(log n)**.
- Ao final, `recalcular_fim` percorre a lista uma vez (O(n)) para atualizar o ponteiro `fim`, o que não muda a ordem total.

### Insertion Sort: O(n²)

Cada nó é encaixado numa lista já ordenada. Antes de percorrer, o código testa dois atalhos: "é maior ou igual ao último?" (vai para o fim) e "é menor que o primeiro?" (vai para o início).

- **Melhor caso, O(n):** lista já ordenada **ou** em ordem inversa: todo nó cai num dos atalhos, com no máximo 2 comparações cada.
- **Pior caso e caso médio, O(n²):** com dados misturados, o nó i pode precisar percorrer até i nós da lista ordenada: 1 + 2 + … + (n−1) = n(n−1)/2 comparações.

### Custo de cada comparação

Comparar IDs é O(1). Comparar títulos/categorias passa por `texto_comparar`, que dobra os dois textos (O(m)). Então, rigorosamente, ordenar por texto custa O(n log n · m) no Merge e O(n² · m) no Insertion. Como m é limitado, a ordem de crescimento em função de n é a da tabela.

## Medição prática

O teste `teste_complexidade` (em [`testes/testes.c`](../testes/testes.c)) embaralha n IDs com semente fixa e conta as comparações feitas por cada algoritmo (`make test`):

| n | Merge Sort | Insertion Sort | n · log₂ n | n² / 4 |
|---:|---:|---:|---:|---:|
| 100 | 541 | 2.471 | 600 | 2.500 |
| 1.000 | 8.712 | 251.420 | 9.000 | 250.000 |
| 4.000 | 42.859 | 4.007.895 | 44.000 | 4.000.000 |

O Merge Sort acompanha n·log₂ n e o Insertion Sort acompanha n²/4 (o número médio esperado de comparações dele em dados aleatórios). Quando n quadruplica (1.000 → 4.000), o Merge faz cerca de 4,9× mais comparações e o Insertion cerca de 16× mais, como a teoria prevê.

Com os 14 servicos reais, no menu: o Merge Sort fez 37 comparações por título; o Insertion Sort fez 56 com a lista misturada e só 13 (n − 1) com a lista já ordenada, que é o melhor caso O(n) (ver [`testes/saidas/04_busca_ordenacao.txt`](../testes/saidas/04_busca_ordenacao.txt)).
