/** Declara as funções relacionadas à Memoria, e a própria memória.
    Mais informações sobre o funcionamento estão no arquivo "Memoria.c" **/

///Status: Pronto

#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED

#include "stdias.h"

extern int40_t memoria[]; //Será definida no módulo Memoria

void ZerarMemoria(); //Inicia a memoria com zeros

void LerMemoria(); //Leitura no estilo IAS

void GravarMemoria(); //Gravação no estilo IAS

void GravarMem(uint12_t endereco, int40_t memo); //Substitui todos os bits da memoria[x]

#endif // MEMORIA_H_INCLUDED
