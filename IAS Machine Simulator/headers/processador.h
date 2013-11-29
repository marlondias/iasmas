/** Declara as funções relacionadas ao Processador.
    Mais informações sobre o funcionamento estão no arquivo "Processador.c" **/

/// Status: Pronto

#ifndef PROCESSADOR_H_INCLUDED
#define PROCESSADOR_H_INCLUDED

void ZerarRegistradores(); //Coloca zero nos registradores

int Processador(int inicioInstrucoes, FILE* logfile, int flagS, int flagR, int flagP, int valorP[]); //Chama UC e emite informações

#endif // PROCESSADOR_H_INCLUDED
