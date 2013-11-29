/// Este arquivo contem testes e não está associado com a versão final do projeto.

#include <stdio.h>
#include "../headers/registradores.h"
#include "../headers/uc.h"
#include "../headers/info.h"
#include "../headers/ula.h"
#include "../headers/memoria.h"

int masdfsdfdin(){
    ZerarMemoria();

    reg_MBR.valor = 0L;
    reg_AC.valor = 0L;
    reg_MQ.valor = 0L;

    reg_IBR.valor = 0;
    reg_MAR.valor = 0;
    reg_IR = 0;
    reg_PC.valor = 0;

    //printf("MBR: %ld AC: %ld MQ: %ld\n", reg_MBR.valor, reg_AC.valor, reg_MQ.valor);

    return 33;
}
