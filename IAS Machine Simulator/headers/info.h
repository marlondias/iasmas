/// Declara funções que mostram ou guardam informações sobre a execução.

///Status: Pronto

#ifndef INFO_H_INCLUDED
#define INFO_H_INCLUDED

#include <stdio.h>

void PrintRegs();

void PrintRegsAll();

void PrintMem(int faixa[]);

void LogRegs(FILE* arquivo);

void LogMem(FILE* arquivo, int faixa[]);

#endif // INFO_H_INCLUDED
