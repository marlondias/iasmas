/** Este arquivo contém o codigo do módulo de tradução/conversão das instruçoes IAS para valores hexadecimal.

    O programa procura pelo arquivo de origem, fornecido pelo módulo "InterfaceSimulador", e cria um arquivo
    de saida com o mesmo nome e a extensão ".hex". Neste novo arquivo, são gravados numeros correspondentes
    às instruçoes IAS ou valores numericos.
    A função Tradutor retorna 0 se ocorrer um erro e 1 se a tradução ocorreu normalmente.

    Obs: Foi adicionada uma instrução EXIT (42) que serve para indicar à UC quando o programa chegou ao fim.
    Mais sobre isto na descrição da Unidade de Controle. **/

///Status: Pronto

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> //strtoll

#include "../headers/stdias.h"

#define TAM_MAX_LINHA 100

void RemoverEspacos(char* entrada, char saida[]){ /// Remove os caracteres " " e "\n" de uma string.
    char* linha = entrada;
    int a = 0; //indice para SAIDA
    while (strcmp(linha, "") != 0){ //utilizar uma comparação mais eficiente (!)
        if ((linha[0] == ' ') || (linha[0] == '\n')) linha++; //ignora esses cararacteres
        else {
            saida[a] = tolower(linha[0]); //armazena os outros caracteres (ja em lowercase)
            linha++;
            a++;
        }
    }
    saida[a] = '\0'; //fecha a string com null
    return;
}


int ReconhecerTipo(const char entrada[], int numeroLinha){
    /** Analisa o conteudo da string ENTRADA e retorna um valor.
        0 = erro
        1 = valor numérico
        2 = <code>
        3 = <data>
        4 = instrução pura (sem endereço de memoria)
        5 = (possivel) instrução com endereço

        Pode emitir mensagens de erro. **/

    if ((isdigit(entrada[0])) || (((entrada[0] == '+') || (entrada[0] == '-')) && (isdigit(entrada[1]))) ){ //numero OU sinal seguido por numero..
        char* pfim;
        strtoll(entrada, &pfim, 10); //Ja que tem um numero aqui, PFIM recebe o que tem depois dele
        if (strcmp(pfim, "") != 0){
            fprintf(stderr, "Erro: O valor numérico possui caracteres a mais ou não é decimal. Linha %d.\n", numeroLinha);
            return 0; //haviam chars depois do numero
        }
        else {
            return 1; //é um numero mesmo!
        }
    }

    else if ((strlen(entrada) < 3) || (strlen(entrada) > 18)) return 0; //Entrada muito pequena OU muito grande para ser instrução ou tag

    else {
        //Não começa em numero, e tem tamanho para ser instrução ou tag..
        int tamanhoEntrada = strlen(entrada);
        if (tamanhoEntrada == 6){
            if (strcmp(entrada, "<code>") == 0) return 2; //é tag
            else if (strcmp(entrada, "<data>") == 0) return 3; //é outra tag
            else if (strcmp(entrada, "loadmq") == 0) return 4; //é instrução pura
            else {
                fprintf(stderr, "Erro: Instrução não reconhecida. Linha %d.\n", numeroLinha);
                return 0; //não existe outra instrução valida com 6 chars
            }
        }
        else if (tamanhoEntrada == 3){
            if (strcmp(entrada, "lsh") == 0) return 4; //é instrução pura
            else if (strcmp(entrada, "rsh") == 0) return 4;
            else {
                fprintf(stderr, "Erro: Instrução não reconhecida. Linha %d.\n", numeroLinha);
                return 0; //não existem outras instruções com 3 chars
            }
        }
        else if (tamanhoEntrada == 4){
            if (strcmp(entrada, "exit") == 0) return 4; //instrução pura
            else {
                fprintf(stderr, "Erro: Instrução não reconhecida. Linha %d.\n", numeroLinha);
                return 0; //não existe outra instrução valida com 4 chars
            }
        }
        else {
            return 5; //Pode ser uma instrução com endereço ou um erro, será analisado novamente..
        }
    }
}


int64_t GerarNumero(const char entrada[]){ ///Retorna um numero a partir de uma string. A ENTRADA deve ser um número válido.
    return (strtoll(entrada, NULL, 10)); //retorna um decimal
}


uint32_t ObterInstrucaoPura(const char entrada[]){ ///Retorna uma palavra de 32bits (20 usados) correspondente à instrução e endereço 00.
    uint32_t saida = 0;
    if (strcmp(entrada, "lsh") == 0){
        saida = 20; //opcode
    }
    else if (strcmp(entrada, "rsh") == 0){
        saida = 21; //opcode
    }
    else if (strcmp(entrada, "loadmq") == 0){
        saida = 10; //opcode
    }
    else if (strcmp(entrada, "exit") == 0){ //instrução extra
        saida = 42; //opcode da instrução extra..
    }

    saida = saida << 12; //e o endereço continua 00
    return saida;
}


int32_t ObterInstrucao(char* entrada, int numeroLinha){
    /** Procura por sequências de texto para definir a instrução, retorna tudo em um numero de 32bit (20 usados).
        Atenção: Existe um modo de "hackear" esta função, mas isto será desconsiderado. **/

    uint8_t opcode;
    uint16_t endereco; //os 4 bits a mais não interferem

    int32_t saida;

    char* linha = entrada;
    char* posEndereco;

    if (strstr(linha, "addm(") != NULL){ //procura uma sequencia que esta (supostamente) no inicio
        linha += 5; //pula os caracteres (supostamente) ja analisados
        endereco = strtol(linha, &posEndereco, 10); //consigo o ENDERECO e um ponteiro para o que tiver depois
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){
            opcode = 5;
        }
        else {
            fprintf(stderr, "Erro: Instrução ADD contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }
    else if (strstr(linha, "add|m(") != NULL){
        linha += 6; //pula os caracteres
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")|") == 0){
            opcode = 7;
        }
        else {
            fprintf(stderr, "Erro: Instrução ADD contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "subm(") != NULL){ //procura uma sequencia
        linha += 5; //pula os caracteres
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){
            opcode = 6;
        }
        else {
            fprintf(stderr, "Erro: Instrução SUB contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }
    else if (strstr(linha, "sub|m(") != NULL){
        linha += 6; //pula os caracteres
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")|") == 0){ //fechamento desejado
            opcode = 8;
        }
        else {
            fprintf(stderr, "Erro: Instrução SUB contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "mulm(") != NULL){
        linha += 5; //pula os ja analisados
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){
            opcode = 11;
        }
        else {
            fprintf(stderr, "Erro: Instrução MUL contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "divm(") != NULL){
        linha += 5; //pula os ja analisados
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){
            opcode = 12;
        }
        else {
            fprintf(stderr, "Erro: Instrução DIV contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "jumpm(") != NULL){
        linha += 6;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ",0:19)") == 0){ //final desejado 1
            opcode = 13;
        }
        else if (strcmp(posEndereco, ",20:39)") == 0){ //final desejado 2
            opcode = 14;
        }
        else { //final indesejado
            fprintf(stderr, "Erro: Instrução JUMP contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "jump+m(") != NULL){
        linha += 7;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ",0:19)") == 0){ //final desejado 1
            opcode = 15;
        }
        else if (strcmp(posEndereco, ",20:39)") == 0){ //final desejado 2
            opcode = 16;
        }
        else { //final indesejado
            fprintf(stderr, "Erro: Instrução JUMP+ contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(linha, "storm(") != NULL){
        linha += 6;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ",8:19)") == 0){ //final desejado 1
            opcode = 18;
        }
        else if (strcmp(posEndereco, ",28:39)") == 0){ //final desejado 2
            opcode = 19;
        }
        else if (strcmp(posEndereco, ")") == 0){ //final desejado 3
            opcode = 33;
        }
        else { //final indesejado
            fprintf(stderr, "Erro: Instrução STOR contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(entrada, "loadmq,m(") != NULL){
        linha += 9;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){
            opcode = 9;
        }
        else {
            fprintf(stderr, "Erro: Instrução LOAD MQ,M(x) contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }


    else if (strstr(entrada, "loadm(") != NULL){
        linha += 6;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){ //fechamento desejado
            opcode = 1;
        }
        else {
            fprintf(stderr, "Erro: Instrução LOAD M(x) contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(entrada, "load-m(") != NULL){
        linha += 7;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")") == 0){ //fechamento desejado
            opcode = 2;
        }
        else {
            fprintf(stderr, "Erro: Instrução LOAD -M(x) contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(entrada, "load|m(") != NULL){
        linha += 7;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")|") == 0){ //fechamento desejado
            opcode = 3;
        }
        else {
            fprintf(stderr, "Erro: Instrução LOAD |M(x)| contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else if (strstr(entrada, "load-|m(") != NULL){
        linha += 8;
        endereco = strtol(linha, &posEndereco, 10);
        if ((endereco < 0) || (endereco > 4095)){
            fprintf(stderr, "Erro: Valor de endereço invalido. Linha %d\n", numeroLinha);
            return -1; //overflow
        }
        if (strcmp(posEndereco, ")|") == 0){ //fechamento desejado
            opcode = 4;
        }
        else {
            fprintf(stderr, "Erro: Instrução LOAD -|M(x)| contém erros. Linha %d\n", numeroLinha);
            return -1; //erro qualquer
        }
    }

    else {
        return -1;
    }

    saida = opcode;
    saida = saida << 12; //desloca o opcode para a posição correta, criando 000 no final do numero
    saida += endereco; //soma o endereço nestes zeros (se for maior que 12 bit não chegará aqui embaixo)
    return saida; //thats it
}


int Tradutor(char* caminhoArquivo, char* nomeArquivo){

    char nomeArquivoIas[TAM_MAX_LINHA] = ""; //Prepara os nomes de arquivo
    strcat(nomeArquivoIas, caminhoArquivo); //com caminho
    strcat(nomeArquivoIas, nomeArquivo); //nome
    strcat(nomeArquivoIas, ".ias"); //e extensão

    char nomeArquivoHex[TAM_MAX_LINHA] = ""; //Prepara os nomes de arquivo
    strcat(nomeArquivoHex, caminhoArquivo);
    strcat(nomeArquivoHex, nomeArquivo);
    strcat(nomeArquivoHex, ".hex");

    FILE *iasfile;
    iasfile = fopen(nomeArquivoIas, "r"); //tenta abrir o .IAS para leitura
    if (iasfile == NULL){
        fprintf(stderr, "Erro: Arquivo \"%s\" inexistente ou acesso negado!\n", nomeArquivoIas);
        return 0; //não existe ou não permite leitura
    }

    FILE *hexfile;
    hexfile = fopen(nomeArquivoHex, "w"); //cria ou abre um .HEX para gravação
    if (hexfile == NULL){
        fprintf(stderr, "Erro: Arquivo \"%s\" não pôde ser criado!\n", nomeArquivoHex);
        return 0; //não permite criação ou escrita
    }

    //Os arquivos estão abertos. Party time!!

    char linhaIas[TAM_MAX_LINHA]; //receberá o stream de texto do arquivo IAS
    int indiceLinha = 1; //indica a linha que está sendo lida
    int flagCode = 0, flagData = 0, tags = 0;
    int40_t numeroIAS; //numero que será usado para receber as instruçoes ou valores convertidos

    while ( fgets(linhaIas, sizeof(linhaIas), iasfile) != NULL ){  //Recebe linha por linha até o fim..
        RemoverEspacos(linhaIas, linhaIas); //remove os espaços e guarda em si mesma (amor proprio..)

        if (strcmp(linhaIas, "") == 0){ //se apos RemoverEspacos() não sobrou nada, ignore a linha
            indiceLinha++;
        }
        else {
            if (ReconhecerTipo(linhaIas, indiceLinha) == 0){
                return 0; //ocorreu um erro que ja foi avisado, cancelar tradutor..
            }

            else if (ReconhecerTipo(linhaIas, indiceLinha) == 1){
                if (flagData == 0){
                    fprintf(stderr, "Erro: Valores numéricos devem estar abaixo da declaração <data>. Linha %d\n", indiceLinha);
                    return 0;
                }
                if ((GerarNumero(linhaIas) > 549755813887) || (GerarNumero(linhaIas) < -549755813888)){
                    fprintf(stderr, "Aviso: Overflow detectado na linha %d.\n", indiceLinha);
                    //Overflow de valores será permitido (porque em programação sempre é..)
                }
                numeroIAS.valor = GerarNumero(linhaIas);
                if (numeroIAS.valor < 0) fprintf(hexfile, "-%lX\n", -numeroIAS.valor); //%X não reconhece hexadecimal negativo..
                else fprintf(hexfile, "%lX\n", numeroIAS.valor); //mas essa gambiarra resolve o problema!
            }

            else if (ReconhecerTipo(linhaIas, indiceLinha) == 2){
                tags++;
                if (tags > 2){
                    fprintf(stderr, "Erro: Há muitas declarações de região. Linha %d\n", indiceLinha);
                    return 0;
                }
                flagCode = 1;
                flagData = 0;
                fprintf(hexfile, "%s\n", linhaIas);
            }

            else if (ReconhecerTipo(linhaIas, indiceLinha) == 3){
                tags++;
                if (tags > 2){
                    fprintf(stderr, "Erro: Há muitas declarações de região. Linha %d\n", indiceLinha);
                    return 0;
                }
                fprintf(hexfile, "%s\n", linhaIas);
                flagCode = 0;
                flagData = 1;
            }

            else if (ReconhecerTipo(linhaIas, indiceLinha) == 4){ //se a linha é tipo 4, pode retornar a instrução com segurança
                if (flagCode == 0){
                    fprintf(stderr, "Erro: Instruções devem estar abaixo da declaração <code>. Linha %d\n", indiceLinha);
                    return 0; //cancela tradução
                }
                fprintf(hexfile, "%X\n", (uint32_t)ObterInstrucaoPura(linhaIas));
                //funciona!
            }

            else if (ReconhecerTipo(linhaIas, indiceLinha) == 5){ //se a linha é tipo 5, são necessarios mais alguns testes..
                if (flagCode == 0){
                    fprintf(stderr, "Erro: Instruções devem estar abaixo da declaração <code>. Linha %d\n", indiceLinha);
                    return 0; //cancela tradução
                }
                if (ObterInstrucao(linhaIas, indiceLinha) < 0) return 0; //cancela tradução
                else {
                    fprintf(hexfile, "%X\n", (uint32_t)ObterInstrucao(linhaIas, indiceLinha));
                }
                //funciona tbm!
            }
            indiceLinha++;
        }
    }

    fclose(iasfile);  //terminou fecha tudo..
    fclose(hexfile);
    return 1; //Sucesso!!!!
}
