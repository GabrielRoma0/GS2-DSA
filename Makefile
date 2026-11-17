# Citizen Assist AI (DSA, GS 2026.2)
# make            compila o programa ./citizen
# make run        compila e abre o menu
# make test       roda os testes automaticos com AddressSanitizer (acusa vazamento)
# make roteiros   roda os roteiros de testes/roteiros e grava as saidas em testes/saidas
# make clean      apaga os binarios

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -O2
SAN     = -std=c11 -Wall -Wextra -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
FONTES  = src/servico.c src/lista.c src/ordenacao.c
CABEC   = src/servico.h src/lista.h src/ordenacao.h

citizen: $(FONTES) src/main.c $(CABEC)
	$(CC) $(CFLAGS) -o citizen $(FONTES) src/main.c

run: citizen
	./citizen

testes/testes_bin: $(FONTES) testes/testes.c $(CABEC)
	$(CC) $(SAN) -o testes/testes_bin $(FONTES) testes/testes.c

test: testes/testes_bin
	./testes/testes_bin

roteiros:
	$(CC) $(SAN) -o citizen_san $(FONTES) src/main.c
	sh testes/rodar_roteiros.sh ./citizen_san

clean:
	rm -f citizen citizen_san testes/testes_bin citizen.exe

.PHONY: run test roteiros clean
