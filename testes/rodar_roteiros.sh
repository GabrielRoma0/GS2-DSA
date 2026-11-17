#!/bin/sh
# Roda cada roteiro de testes/roteiros no menu e grava a saida em testes/saidas.
# Uso: sh testes/rodar_roteiros.sh [./citizen]
PROG=${1:-./citizen}
mkdir -p testes/saidas
for r in testes/roteiros/*.txt; do
  nome=$(basename "$r" .txt)
  "$PROG" < "$r" > "testes/saidas/$nome.txt" 2>&1
  echo "$nome: saida em testes/saidas/$nome.txt (codigo $?)"
done
