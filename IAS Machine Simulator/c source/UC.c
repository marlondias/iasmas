/** Este código simula a Unidade de Controle do IAS.
    O fluxo é dividido nas fases de Busca e de Execução. Representadas por funções com o mesmo nome.
    A fase de busca é dividida em mais duas fases, pois a instrução pode estar à esquerda ou direita.

    A função CicloExecucao() verifica o opcode que esta em IR, move dados se necessário (MAR, MBR..) e
    muda o valor das flags de acordo com a instrução. Se o opcode for desconhecido, a função retorna 0.
    Se o opcode for 42 (instrução END) a execução é terminada.

    Testados:
        LOADs


**/

///Status: Pronto

#include <stdio.h>
#include <stdlib.h> //só pelo LABS

#include "../headers/memoria.h"
#include "../headers/registradores.h"
#include "../headers/ula.h"

uint8_t reg_IR; //Instruction Register
uint20_t reg_IBR; //Instruction Buffer Register
uint12_t reg_PC; //Program Counter
uint12_t reg_MAR; //Memory Address Register

static int flagDesvio = 0; //Mostra se houve desvio no fluxo de execução
static int flagDireita = 0; //Indica se a proxima instrução é da direita
static int logicoAritmeticas = 0, saltosCondicionais = 0, saltosIncondicionais = 0, transferenciaDados = 0, modEndereco = 0; //Contadores
//Muitas globais? What ur talkin bout?!


static void CicloBuscaEsquerda(){ ///Busca por pares de instruções na memória
    reg_MAR.valor = reg_PC.valor; //MAR <= PC
    LerMemoria(); //MBR <= M(MAR)
    reg_IBR.valor = reg_MBR.valor; //IBR <= MBR(20:39) -- O corte dos bits é automático
    reg_MBR.valor = reg_MBR.valor >> 20; //Desloca até a metade esquerda de MBR
    reg_MAR.valor = reg_MBR.valor; //MAR <= MBR(28:39) que na verdade é MBR(8:19)
    reg_MBR.valor = reg_MBR.valor >> 12;
    reg_IR = reg_MBR.valor; //IR <= MBR(20:27) que na verdade é MBR(0:7)
    reg_PC.valor++; //PC = PC + 1
}


static void CicloBuscaDireita(){ ///Busca por uma instrução na parte direita da memória
    if (flagDesvio){
        //com desvio no fluxo..
        reg_MAR.valor = reg_PC.valor; //MAR <= PC
        LerMemoria(); //MBR <= M(MAR)
        reg_MAR.valor = reg_MBR.valor; //MAR <= MBR(28:39) -- Lembrando que o corte dos bits é automatico
        reg_MBR.valor = reg_MBR.valor >> 12; //desloca para conseguir os 8 bits
        reg_IR = reg_MBR.valor; //IR <= MBR(20:27)
        reg_PC.valor++; //PC = PC + 1
    }
    else {
        //sem desvio..
        reg_MAR.valor = reg_IBR.valor; //MAR <= IBR(8:19)
        reg_IBR.valor = reg_IBR.valor >> 12; //desloca pra conseguir os 8 bits de cima
        reg_IR = reg_IBR.valor; //IR <= IBR(0:7)
    }
    flagDesvio = 0;
    return;
}


static int CicloExecucao(){ ///Decodifica, busca os operandos e executa
    flagDesvio = 0; //por padrão não houve desvio
    int40_t auxReg;
    auxReg.valor = 0L;

    switch(reg_IR){
        case(1):
            //load m(x)
            transferenciaDados++;
            LerMemoria(); //MBR <= M(MAR)
            reg_AC.valor = reg_MBR.valor; //AC <= MBR
            break;
        case(2):
            //load -m(x)
            transferenciaDados++;
            LerMemoria(); //MBR <= M(MAR)
            reg_AC.valor = -(reg_MBR.valor); //AC <= -MBR
            break;
        case(3):
            //load |m(x)|
            transferenciaDados++;
            LerMemoria(); //MBR <= M(MAR)
            reg_AC.valor = labs(reg_MBR.valor); //AC <= |MBR|
            break;
        case(4):
            //load -|m(x)|
            transferenciaDados++;
            LerMemoria(); //MBR <= M(MAR)
            reg_AC.valor = -(labs(reg_MBR.valor)); //AC <= -|MBR|
            break;

        case(5):
            //add m(x)
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(0); //AC <= AC + MBR
            break;
        case(6):
            //sub m(x)
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(2); //AC <= AC - MBR
            break;
        case(7):
            //add |m(x)|
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(1); //AC <= AC + |MBR|
            break;
        case(8):
            //sub |m(x)|
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(3); //AC <= AC - |MBR|
            break;

        case(9):
            //load mq m(x)
            transferenciaDados++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(8); //MQ <= MBR
            break;
        case(10):
            //load mq
            transferenciaDados++;
            ULA(9); //AC <= MQ
            break;

        case(11):
            //mul m(x)
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(4); // AC.MQ <= MBR * MQ -- msb em AC e lsb em MQ
            break;
        case(12):
            //div m(x)
            logicoAritmeticas++;
            LerMemoria(); //MBR <= M(MAR)
            ULA(5); //AC.MQ <= AC/MBR -- quociente em MQ e resto em AC
            break;

        case(13):
            saltosIncondicionais++;
            reg_PC.valor = reg_MAR.valor; //PC <= MAR
            flagDireita = 0; //pois vai pular para uma esquerda
            //flagDesvio = 1; //pois foi um jump
            break;
        case(14):
            saltosIncondicionais++;
            reg_PC.valor = reg_MAR.valor; //PC <= MAR
            flagDireita = 1; //pois vai pular para uma direita
            flagDesvio = 1; //pois foi um jump
            break;

        case(15):
            saltosCondicionais++;
            if (reg_AC.valor >= 0){ //AC não negativo
                reg_PC.valor = reg_MAR.valor; //PC <= MAR
                flagDireita = 0; //pois vai pular para uma esquerda
                //flagDesvio = 1; //pois foi um jump
            }
            break;
        case(16):
            saltosCondicionais++;
            if (reg_AC.valor >= 0){
                reg_PC.valor = reg_MAR.valor; //PC <= MAR
                flagDireita = 1; //pois vai pular para uma direita
                flagDesvio = 1; //pois foi um jump
            }
            break;

        case(18):
            //stor m(x, 8:19)
            modEndereco++;
            LerMemoria(); //MBR <= M(MAR) -- Copia o valor original de m(x)
            reg_MBR.valor = reg_MBR.valor & 1095217709055; //Anula os 12 bits do meio de MBR
            auxReg.valor = reg_AC.valor & 4293918720; //AUX recebe apenas os 12 bits do meio de AC
            reg_MBR.valor += auxReg.valor; //junta e retorna tudo para o MBR
            GravarMemoria(); //M(MAR) <= novo MBR
            break;
        case(19):
            //stor m(x, 28:39)
            modEndereco++;
            LerMemoria(); //MBR <= M(MAR) -- Copia o valor original de m(x)
            reg_MBR.valor = reg_MBR.valor & 1099511623680; //Anula os 12 ultimos bits de MBR
            auxReg.valor = reg_AC.valor & 4095; //AUX recebe apenas os 12 bits finais de AC
            reg_MBR.valor += auxReg.valor; //junta e retorna tudo para o MBR
            GravarMemoria(); //M(MAR) <= novo MBR
            break;

        case(20):
            //lsh
            logicoAritmeticas++;
            ULA(6); //AC <= AC << 1
            break;
        case(21):
            //rsh
            logicoAritmeticas++;
            ULA(7); //AC <= AC << 1
            break;

        case(33):
            //stor m(x)
            transferenciaDados++;
            reg_MBR.valor = reg_AC.valor; //MBR <= AC
            GravarMemoria(); //M(MAR) <= MBR
            break;
        case(42):
            //instrução EXIT
            return 2; //sucesso e fim da execução do programa
        default:
            return 0; //erro de opcode desconhecido
    }
    return 1; //sucesso por enquanto..
}


void Estatistica(){ ///Exibe resumo das instruções no fim da execução
    int total = (transferenciaDados + logicoAritmeticas + saltosCondicionais + saltosIncondicionais + modEndereco);

    printf("Resumo das instruções executadas:\n");
    printf("Transferências de dados -- %d ( %f %%)\n", transferenciaDados, (100*transferenciaDados)/(float)total );
    printf("Aritméticas             -- %d ( %f %%)\n", logicoAritmeticas, (100*logicoAritmeticas)/(float)total );
    printf("Modficação de endereço  -- %d ( %f %%)\n", modEndereco, (100*modEndereco)/(float)total );
    printf("Saltos condicionais     -- %d ( %f %%)\n", saltosCondicionais, (100*saltosCondicionais)/(float)total );
    printf("Saltos incondicionais   -- %d ( %f %%)\n", saltosIncondicionais, (100*saltosIncondicionais)/(float)total );
    printf("Totalizando %d instruções.\n", total);
}


int UC(){ ///Realiza um ciclo completo (B, D, BO, E)
    int status; //valor retornado pela Execução
    if (flagDireita == 0){ //se a proxima instrução não estiver na direita..
        CicloBuscaEsquerda();
        flagDireita = 1; //indica que a proxima será da direita
    }
    else {
        CicloBuscaDireita();
        flagDireita = 0; //indica que a proxima será da esquerda
    }
    status = CicloExecucao(); //pode modificar as flags e retorna um codigo de estado
    return (status); //retorna o retorno da execução
}
