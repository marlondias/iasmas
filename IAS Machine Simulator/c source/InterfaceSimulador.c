/** Este é o código que define o comportamento do simulador em relação às ações do usuário.
    Todas as opções disponíveis ao usuário são tratadas aqui, assim como a maioria dos erros.
    É a partir daqui que ocorrem as chamadas às funções de tradução, carregamento e execução. **/

///Status: Pronto

#include <stdio.h>
#include <stdlib.h> //strtol
#include <unistd.h> //getopt
#include <string.h>

#include "../headers/tradutor.h"
#include "../headers/carregador.h"
#include "../headers/info.h" //printmem
#include "../headers/uc.h" //estatistica
#include "../headers/processador.h"

#define TAM_MAX_LINHA 100


static int InterpretaAlvo(char* entrada){ ///OK
    /**Analisa um nome de arquivo e retorna um codigo:
        0 se o nome é invalido
        1 se o nome possui a extensão.IAS
        2 se o nome possui um caminho e a extensão .IAS **/

//falta analisar os casos: ".ias" -- só extensão   "/nome.ias" -- barra no inicio   "/.ias" -- ambos os erros

    if (strcmp(entrada, "") == 0) return 0; //o alvo é nulo
    char* teste = entrada;
    int barras = 0;
    while(strcmp(teste, "") != 0){ //procura por simbolos proibidos e por "/"
        if (teste[0] == '/'){
            barras++;
            teste--;
            if (teste[0] == '/') return 0; //verifica se a anterior também é uma barra
            teste++; //volta ao normal
        }
        //futuramente, verificar simbolos proibidos
        teste++;
    }
    teste -= 4;
    if ((barras == 0) && (strcmp(teste, ".ias") == 0)) return 1; //não tem caminho e a extensão é IAS
    else if ((barras > 0) && (strcmp(teste, ".ias") == 0)) return 2; //tem caminho e a extensão é IAS
    else return 0;
}


static void ExtrairNome(char* entrada, char* saida){ ///OK
    /** Recebe um nome de arquivo e retorna apenas o nome.
        Não retorna nada se o arquivo for inválido.
        O retorno fica na string SAIDA **/

    if (InterpretaAlvo(entrada) == 0){
        return;
    }

    char* teste = entrada;
    int barras = 0;
    while(strcmp(teste, "") != 0){ //procura por simbolos "/"
        if (teste[0] == '/') barras++;
        teste++;
    }

    teste = entrada; //denovo
    if (barras > 0){
        while(barras > 0){ //remove as barras e tudo que elas representam
            if (teste[0] == '/') barras--;
            teste++;
        }
    }

    int i = 0;
    while (strcmp(teste, ".ias")){
        saida[i] = teste[0];
        teste++;
        i++;
    }
    saida[i] = '\0'; //fecha com null
    return;
}


static void ExtrairCaminho(char* entrada, char* saida){ ///OK
    /** Recebe um nome de arquivo que contem um caminho, e retorna apenas o caminho.
        Só funciona para arquivos válidos do tipo 2 (possui caminho no nome).
        O retorno vai para a string SAIDA. **/

    if (InterpretaAlvo(entrada) != 2) return;

    int barras = 0, tamanhoNome = 0; //contadores
    char* teste = entrada;

    while(strcmp(teste, "") != 0){ //conta os simbolos "/" e determina o tamanho do nome do aquivo (com extensão)
        if (teste[0] == '/'){
            tamanhoNome = 0; //reinicia a contagem se encontrar uma barra
            barras++;
        }
        else {
            tamanhoNome++;
        }
        teste++;
    }
    teste = entrada;
    int i = 0;
    while (barras > 0){
        if (teste[0] == '/') barras--; //quando passar por todas as "/" sobrará o filename
        saida[i] = teste[0];
        teste++;
        i++;
    }
    //aponta para a ultima barra e substitui com null
    saida[i] = '\0';
    return;
}


int main(int argc, char** argv){ ///OK
    int flagS = 0, flagR = 0, flagL = 0, flagM = 0, flagP = 0, flagErro = 0, flagHelp = 0; //boleanos

    int valorM; //recebe o argumento de M
    int valorP[2]; //recebem o argumento de P
    int opcao; //recebe a saida de getopt

    opterr = 0; //desliga erros default
    while ( (opcao = getopt(argc, argv, "hsrlm:p:")) != -1 ){ //interpreta os argumentos do terminal
        long int valor1 = 0,valor2 = 0; //auxiliares
        char* auxptr;

        switch(opcao){
            case 'h': //help
                flagHelp = 1;
                break;
            case 's': //silencioso
                flagS = 1;
                break;
            case 'r': //registradores
                flagR = 1;
                break;
            case 'l': //log
                flagL = 1;
                break;

            case 'm': //memoria maxima
                valor1 = strtol(optarg, &auxptr, 10);
                if ( strcmp(auxptr,"") != 0){
                    fprintf(stderr, "Erro: Valor de memória inválido\n"); //pq não aceita null?!
                    flagErro = 1;
                }
                else if (valor1 < 1){
                    fprintf(stderr, "Erro: O valor de memória mínimo é 1\n");
                    flagErro = 1;
                }
                else if (valor1 > 4096){
                    fprintf(stderr, "Erro: O valor de memória máximo é 4096\n");
                    flagErro = 1;
                }
                else {
                    valorM = valor1;
                    flagM = 1;
                }
                break;

            case 'p': //partição
                valor1 = strtol(optarg, &auxptr, 10);
                if ( strcmp(auxptr,"") == 0 ){
                    fprintf(stderr, "Erro: Apenas um valor foi especificado em -p\n");
                    fprintf(stderr, " *Use -p valor1/valor1 para observar um único endereço\n");
                    flagErro = 1;
                }
                else if ( auxptr[0] != '/'){
                    fprintf(stderr, "Erro: Os valores em -p devem ser separados por \"/\"\n");
                    flagErro = 1;
                }
                else {
                    auxptr++; //pula a barrinha
                    valor2 = strtol(auxptr, &auxptr, 10);
                    if ( strcmp(auxptr, "") != 0){
                        fprintf(stderr, "Erro: Valor inválido no argumento de -p\n");
                        flagErro = 1;
                    }
                    else {
                        if ( (valor1 > -1) && (valor1 < 4096) && (valor2 < 4096) ){
                            if (valor1 <= valor2){
                                valorP[0] = valor1;
                                valorP[1] = valor2;
                                flagP = 1;
                            }
                            else {
                                fprintf(stderr, "Erro: Os valores em -p devem estar em ordem crescente\n");
                                flagErro = 1;
                            }
                        }
                        else {
                            fprintf(stderr, "Erro: O intervalo especificado em -p ultrapassa os limites da memória\n");
                            flagErro = 1;
                        }
                    }
                }
                break;

            case '?': //erro
                if(optopt == 'm'){
                    fprintf(stderr, "Erro: Falta o argumento para -m\n");
                    fprintf(stderr, "Uso: -m <valor>\n");
                    flagErro = 1;
                }
                else if(optopt == 'p'){
                    fprintf(stderr, "Erro: Falta o argumento para -p\n");
                    fprintf(stderr, "Uso: -p <valor1/valor2>\n");
                    flagErro = 1;
                }
                else{
                    fprintf(stderr, "Erro: Opção -%c desconhecida\n", optopt);
                    fprintf(stderr, "Use -h para obter ajuda\n");
                    flagErro = 1;
                }
                break;
            default:
                fprintf(stderr, "Johnny, la gente esta muy loca!\n"); //Erro totalmente desconhecido (e improvavel)
                flagErro = 1;
        }
    }
//chega de flags!!!

    char* nomeArquivoFull; //recebe o nome completo do arquivo alvo
    char nomeArquivo[TAM_MAX_LINHA] = ""; //recebe apenas o nome do arquivo (sem extensão) -- não pode ser ponteiro
    char caminhoArquivo[TAM_MAX_LINHA] = ""; //recebe apenas o caminho do arquivo (se houver) -- não pode ser ponteiro

    if (argc > (optind+1)){
        fprintf(stderr, "Erro: Há mais parametros que o esperado\n");
        fprintf(stderr, "Uso: iasmas [opções] <arquivo>\n");
        flagErro = 1;
    }
    else if (argc == optind){
        if (flagErro == 0){ //ainda não houve erro e não tem -h..
            if (flagHelp == 0){
                fprintf(stderr, "Erro: Falta o nome do arquivo alvo\n");
                fprintf(stderr, "Uso: iasmas [opções] <arquivo>\n");
                flagErro = 1;
            }
        }
    }
    else {
        nomeArquivoFull = argv[optind];
        if (InterpretaAlvo(nomeArquivoFull) == 0){
            fprintf(stderr, "Erro: O arquivo especificado não é válido\n");
            flagErro = 1;
        }
        else if (InterpretaAlvo(nomeArquivoFull) == 1){
            ExtrairNome(nomeArquivoFull, nomeArquivo);
        }
        else if (InterpretaAlvo(nomeArquivoFull) == 2){
            ExtrairCaminho(nomeArquivoFull, caminhoArquivo);
            ExtrairNome(nomeArquivoFull, nomeArquivo);
        }
    }

//Resultado das flags
    if (flagHelp){
        printf("Uso: iasmas [opçoes] arquivo\n");
        printf("Opções:\n");
        printf(" -h                  Exibe esta informação\n");
        printf(" -s                  Reduz a quantidade de informações durante a execução\n");
        printf(" -l                  Grava as informações sobre a execução em um arquivo de texto\n");
        printf(" -r                  Mostra o estado de todos os registradores a cada ciclo\n");
        printf(" -m <valor>          Define o tamanho da memória acessível (0 a 4096)\n");
        printf(" -p <valor1.valor2>  Define um intervalo de memória que será exibido constantemente\n\n");
        printf("Atenção:\n");
        printf(" A opção -m não afeta os valores permitidos em -p\n"); //valores inacessiveis serão sempre 00
        printf(" A opção -s anula a opção -r\n");
        return 0;
    }

    if (flagErro){
        fprintf(stderr, "Execução abortada!\n");
        return 1;
    }

    if (flagS && flagR) flagR = 0; //não pode ser silencioso com -r

    printf("Condições de simulação:\n");
    printf("\tCaminho de trabalho:   %s\n", caminhoArquivo);
    printf("\tCódigo fonte:          %s.ias\n", nomeArquivo);
    printf("\tCódigo objeto:         %s.hex\n", nomeArquivo);
    if (flagL) printf("\tArquivo de log:        %s.log\n", nomeArquivo);
    printf("\n");
    if (flagM) printf("\tA memória será limitada a %d endereços\n", valorM);
    if (flagP) printf("\tO conteúdo da memória nos endereços %d a %d será monitorado\n", valorP[0], valorP[1]);
    if (flagS) printf("\tSerá exibido o mínimo de informações durante a execução\n");
    if (flagR) printf("\tSerão exibidas informações detalhadas durante a execução\n");


    printf("Continuar? [s/n] "); //pede confirmação para iniciar tradução
    char resposta = getc(stdin);
    if ((resposta == 'n') || (resposta == 'N')){
        printf("\n#partiu\n");
        printf("Execução cancelada pelo usuário!\n");
        return 0;
    }
    else if ((resposta == 's') || (resposta == 'S')){
        printf("\nIniciando geração do código objeto...\n");
        if (Tradutor(caminhoArquivo, nomeArquivo) == 0){
            fprintf(stderr, "Ocorreram erros durante o processo de tradução.\nExecução abortada!\n");
            return 1;
        }
        else printf("Arquivo objeto criado com sucesso!\n\n");
    }
    else {
        printf("\nNenhuma resposta foi fornecida.\n");
        printf("Execução cancelada!\n");
        return 0;
    }


    printf("\nCarregando dados na memoria...\n");
    int inicioPrograma = Carregador(caminhoArquivo, nomeArquivo);
    if (inicioPrograma == -1){
        fprintf(stderr, "Ocorreram erros durante o carregamento.\n");
        return 1; //provavelmente o usuário estragou o HEX..
    }
    else {
        printf("Programa carregado com sucesso!\n");
        printf("Iniciando execução...\n\n");

        FILE *logfile; //só pra não perder o escopo
        if (flagL) { //Prepara o arquivo de log
            char nomeArquivoLog[TAM_MAX_LINHA] = ""; //Prepara o nome de arquivo
            strcat(nomeArquivoLog, caminhoArquivo);
            strcat(nomeArquivoLog, nomeArquivo);
            strcat(nomeArquivoLog, ".log");

            logfile = fopen(nomeArquivoLog, "w"); //cria um .LOG para gravação
            if (logfile == NULL){
                fprintf(stderr, "Erro: Arquivo \"%s\" não pôde ser criado!\nExecução abortada!\n", nomeArquivoLog);
                return 1; //não permite criação ou escrita
            }
            //LOG foi aberto..
        }
        else logfile = NULL;


        int resultadoSim = Processador(inicioPrograma, logfile, flagS, flagR, flagP, valorP);
        if (resultadoSim == 0){
            fprintf(stderr, "Simulação finalizada devido a erros!\n");
            return 1; //só pra ficar um numero vermelho bonitinho no terminal
        }
        else {
            if ((flagP) && (flagS)) PrintMem(valorP); //se estava no modo Silent mostra a partição só no final
            printf("Simulação completa!\n\n");
            Estatistica();
            return 0; //FIM -- Yo yo yo this is it!!
        }

        if (logfile != NULL) fclose(logfile); //se está aberto feche
    }
}
