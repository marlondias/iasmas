/** Este codigo simula uma memória semelhante à do computador IAS.
    São 4096 endereços de 40 bits cada, com o bit mais significativo usado para
    indicar o sinal e 39 bits para compor o valor do número, em complemento de 2.

    A faixa de valores é:
        -549.755.813.888 -> 549.755.813.887

    AVISOS:
        Validação e tratamento de erros serão feitos pela UC ou algo do tipo, a memória apenas recebe ordens.

        Se existir uma instrução com opcode 255 o resultado será 1111 1111 ???? ???? ???? e do modo como foi
        implementado abaixo o valor na memoria ficará negativo. Isso é um BUG!!

        Os valores usados nas ANDs foram calculados usando posicionamento de bits no Kcalc (calculadora do KDE).
        O objetivo é anular determinados bits para criar lugar para outros. **/

///Status: Pronto

#include "../headers/stdias.h"
#include "../headers/registradores.h"

#define TAM_MEMORIA 4096

int40_t memoria[TAM_MEMORIA]; ///Aloca a memoria (estaticamente)

void ZerarMemoria(){ ///Coloca 000 em todos os endereços de memória
    int a;
    for (a = 0; a < TAM_MEMORIA; a++){
        memoria[a].valor = 0L;
    }
}

void LerMemoria() { ///Realiza a leitura de um dado na memória
    //MAR já contem o endereço desejado
    //O valor da MEMORIA nesse endereço é gravado em MBR
    //Ou seja, MBR <= M(MAR)
    reg_MBR.valor = memoria[reg_MAR.valor].valor;
}

void GravarMemoria() { ///Realiza a gravação de um dado na memória
    //MAR já contem o endereço desejado
    //MBR contém o dado a ser armazenado
    //O valor da MEMORIA nesse endereço recebe o valor em MBR
    //Ou seja, M(MAR) <= MBR
    memoria[reg_MAR.valor].valor = reg_MBR.valor;
}

void GravarMem( uint12_t endereco, int40_t memo) {  ///Método alternativo para gravar na memória.
    memoria[endereco.valor].valor = memo.valor;
}
