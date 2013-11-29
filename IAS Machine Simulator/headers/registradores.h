/** Este arquivo deixa os registradores acessíveis para outros arquivos.
    Cada um deles é declarado em sua respectiva unidade (ULA ou UC).

    Foram utilizados tipos com tamanho personalizado em algumas variáveis,
    para oferecer maior fidelidade ao computador IAS original **/

///Status: Pronto

#ifndef REGISTRADORES_H_INCLUDED
#define REGISTRADORES_H_INCLUDED

#include "stdias.h"

//Registradores UC
extern uint8_t reg_IR; //Instruction Register -- 8bit
extern uint20_t reg_IBR; //Instruction Buffer Register -- 20bit
extern uint12_t reg_PC; //Program Counter -- 12bit
extern uint12_t reg_MAR; //Memory Address Register -- 12bit

//Registradores ULA
extern int40_t reg_MBR; //Memory Buffer Register -- 40bit
extern int40_t reg_AC; //Accumulator -- 40bit
extern int40_t reg_MQ; //Multiplier Quotient -- 40bit

#endif // REGISTRADORES_H_INCLUDED
