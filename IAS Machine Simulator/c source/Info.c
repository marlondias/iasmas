/** Este código contém funções para mostrar ou gravar informações diversas durante a execução.
    Favor ignorar os 2743 avisos do compilador.. **/

///Status: Pronto

#include <stdio.h>
#include "../headers/registradores.h"
#include "../headers/memoria.h"

void PrintRegs(){ ///Mostra alguns registradores na tela
    printf("\n");
    printf("AC:  %ld\n", reg_AC.valor);
    printf("MQ:  %ld\n", reg_MQ.valor);
    printf("PC:  %d\n", reg_PC.valor);
    return;
}


void PrintRegsAll(){ ///Mostra todos os registradores na tela
    printf("\n");
    printf("MBR: %ld\n", reg_MBR.valor);
    printf("AC:  %ld\n", reg_AC.valor);
    printf("MQ:  %ld\n", reg_MQ.valor);
    printf("PC:  %d\n", reg_PC.valor);
    printf("IR:  %d\n", reg_IR);
    printf("MAR: %d\n", reg_MAR.valor);
    printf("IBR: %d\n", reg_IBR.valor);
    return;
}


void PrintMem(int faixa[]){ ///Mostra uma faixa da memória na tela
    int a = faixa[0]; //inicio da faixa de valores
    while (a <= faixa[1]){ //enquanto não chegar ao fim da faixa..
        printf("Mem %d: %ld\n", a, memoria[a].valor);
        a++;
    }
    return;
}


void LogRegs(FILE* arquivo){ ///Grava todos os registradores no LOG
    fprintf(arquivo, "-----------------------------------------\n");
    fprintf(arquivo, "MBR: %ld\n", reg_MBR.valor);
    fprintf(arquivo, "AC:  %ld\n", reg_AC.valor);
    fprintf(arquivo, "MQ:  %ld\n", reg_MQ.valor);
    fprintf(arquivo, "PC:  %d\n", reg_PC.valor);
    fprintf(arquivo, "IR:  %d\n", reg_IR);
    fprintf(arquivo, "MAR: %d\n", reg_MAR.valor);
    fprintf(arquivo, "IBR: %d\n", reg_IBR.valor);
    return;
}


void LogMem(FILE* arquivo, int faixa[]){ ///Grava uma faixa da memória no LOG
    int a = faixa[0]; //inicio da faixa de valores
    while (a <= faixa[1]){ //enquanto não chegar ao fim da faixa..
        fprintf(arquivo, "Mem %d: %ld\n", a, memoria[a].valor);
        a++;
    }
    fprintf(arquivo, "\n"); //separador
    return;
}
