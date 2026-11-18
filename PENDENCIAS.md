# ✅ O que falta para tirar 10 em DSA (entrega 18/11)

O código, os testes, os prints, a análise Big-O e a documentação já estão prontos. Faltam 3 coisas, todas da dupla:

## 1. [ ] Entender o código (30 min, os dois)

Na demonstração o professor pode perguntar como funciona. Ler nesta ordem:

1. `src/lista.h`: o nó (`struct No`, com `dado` e `prox`) e a lista (`inicio`, `fim`, `tamanho`).
2. `src/lista.c`:
   - `lista_inserir`: valida, recusa ID repetido, faz `malloc` e liga no fim.
   - `lista_remover`: os 3 casos (primeiro, meio, último) e o `free`.
   - `lista_liberar`: guarda o `prox` antes do `free`.
3. `src/ordenacao.c`:
   - `dividir_ao_meio`: ponteiro lento e ponteiro rápido.
   - `intercalar`: junta duas metades ordenadas.
   - `ordenar_insercao`: encaixa cada nó no lugar certo.
4. `docs/big-o.md`: saber dizer por que inserir é O(n) (a checagem de ID repetido percorre a lista) e por que o Merge Sort é O(n log n).

**Pronto quando:** cada um consegue explicar a remoção e o Merge Sort sem ler.

## 2. [x] Fazer a demonstração (feito: vídeo em https://youtu.be/q3h-G37EPGs, link no README)

> **Vídeo pronto, sem narração:** `video/dsa-demonstracao.mp4` (1:58), com música calma de fundo composta para o vídeo, sem direitos autorais de terceiros (há também `dsa-demonstracao-sem-som.mp4`). Mostra a execução real com textos explicativos na tela; **não precisa gravar voz**. Só assistir uma vez, subir no YouTube como não listado e colar o link no README (passos 4 e 5 abaixo). O `.mp4` não vai para o GitHub (fica só na pasta do computador).

O enunciado pede "demonstração prática" e não exige vídeo. Se a entrega for só pelo Portal, gravar é o jeito seguro de garantir esse 1,0 ponto.

**Se preferirem gravar a tela vocês mesmos** (em vez de usar o vídeo pronto), ou para apresentar ao vivo:

1. Na pasta do projeto, compilar: `make clean && make` (no Windows: o comando `gcc` do README).
2. Seguir o roteiro [`video/roteiro.md`](video/roteiro.md) (cerca de 4 min, com quem fala o quê).
3. Gravar a tela com o **OBS Studio** (grátis, obsproject.com): *Fonte → Captura de tela → Iniciar gravação*.
4. Subir no YouTube como **não listado**: studio.youtube.com → *Criar → Enviar vídeos* → em *Visibilidade*, escolher **Não listado**.
5. Colar o link no `README.md`, na seção "Documentação". Pelo site do GitHub: abrir o arquivo → lápis ✏️ → *Commit changes*.

**Pronto quando:** o link do vídeo abre numa aba anônima.

## 3. [ ] Entregar no Portal

1. Conferir no Portal o formato pedido: link do repositório, ZIP ou `.txt`.
2. Mandar o nome completo e o RM dos dois, o link do repositório (https://github.com/GabrielRoma0/GS2-DSA) e o link do vídeo, se tiver.
   - Se for ZIP: no GitHub, *Code → Download ZIP*.

**Pronto quando:** o Portal mostra a entrega como enviada.
