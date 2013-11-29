/** Responsável por definir a Unidade Central de Processamento.
    Na arquitetura de Von Neumann, a UCP é definida como a combinação da ULA e UC.
    Este código tenta replicar este comportamento. **/

///Status: Pronto

#include "../headers/uc.h"
#include "../headers/registradores.h"
#include "../headers/info.h"
#include "../headers/ula.h" //Desnecessário, mas é uma inclusão "conceitual" ja que a ULA faz parte do processador

void ZerarRegistradores(){ ///Coloca 000 em todos os registradores
    reg_AC.valor = 0L;
    reg_MQ.valor = 0L;
    reg_MBR.valor = 0L;
    reg_IR = 0;
    reg_IBR.valor = 0;
    reg_MAR.valor = 0;
    reg_PC.valor = 0;
    return;
}


int Processador(int inicioInstrucoes, FILE* logfile, int flagS, int flagR, int flagP, int valorP[]){
    /** Esta função não existia, o núcleo da simulação acontecia no arquivo "InterfaceSimulador.c".
        Porém, para manter a fidelidade, o núcleo foi movido para cá, sem muitas modificações.
        Por isso, algumas variaveis tem o mesmo nome e alguns pontos não fazem muito sentido.
        Mas a simulação continua ocorrendo normalmente.  **/

    ZerarRegistradores();
    reg_PC.valor = inicioInstrucoes; //Obtido durante o carregamento.

    int flagL = 0;
    if (logfile != NULL) flagL = 1; //se o ponteiro for válido, então a flagL estava acesa.

    int saidaUC;

    while (1){
        saidaUC = UC(); //ativa UC e guarda o retorno

        if (saidaUC == 2){
            printf("Execução terminada com sucesso!\n\n");
            if (flagL) printf("Os resultados completos foram gravados em um log.\n");
            break; //sai do while
        }
        else if (saidaUC == 0) {
            fprintf(stderr, "Ocorreram erros durante a execução.\n");
            return 0; //erro no codigo IAS
        }

        //Ações depois de cada ciclo..
        if (flagS == 0) {
            //No modo Silent, nada é mostrado durante a execução
            if (flagR) PrintRegsAll(); //mostrar todos os registradores
            else PrintRegs(); //mostrar só os mais relevantes
            if (flagP) PrintMem(valorP); //mostra partição de memoria se necessário
        }

        if (flagL){
            LogRegs(logfile); //registra no log se necessário
            if (flagP) LogMem(logfile, valorP);
        }
    }

    return 1; //sucesso!!
}
