/** Este código é responsável pela transferência de todo o programa IAS à memoria RAM.
    Carrega os valores do arquivo HEX nos endereços definidos no módulo Memoria,
    agrupando em pares em caso de instruções.

    Durante esse processo, o Carregador consegue identificar a primeira posição de memória que
    contém uma instrução IAS. Este valor é retornado para ser utilizado posteriormente.

    Essa função não procura por erros, pois o arquivo HEX será gerado de forma segura e o
    único modo de ocorrer um erro é se o arquivo for alterado após o processo de tradução.

    Retorna o endereço que contém a primeira instrução ou -1. **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> //strtoll

#include "../headers/stdias.h"
#include "../headers/memoria.h"

#define TAM_MAX_LINHA 100

int Carregador(char* caminhoArquivo, char* nomeArquivo){
    ZerarMemoria();

    char nomeArquivoHex[TAM_MAX_LINHA] = ""; //Prepara os nomes de arquivo
    strcat(nomeArquivoHex, caminhoArquivo);
    strcat(nomeArquivoHex, nomeArquivo);
    strcat(nomeArquivoHex, ".hex");

    FILE *hexfile;
    hexfile = fopen(nomeArquivoHex, "r"); //tenta abrir o .HEX para leitura
    if (hexfile == NULL){
        fprintf(stderr, "Erro: Arquivo \"%s\" não pôde ser acessado!\n", nomeArquivoHex);
        return -1; //não permite criação ou escrita
    }

    char linhaHex[TAM_MAX_LINHA]; //receberá as linhas do .hex

    uint12_t indiceEndereco; //indica o endereco a ser usado para gravação
    indiceEndereco.valor = 0;

    int40_t dado; //recebe a palavra de 40bits a ser gravada

    int flagEsquerda = 1; //começa pela esquerda, então ESQUERDA é 1
    int flagCode = 0, flagData = 0;
    int inicioCodigo = 0; //marca o endereço de memoria com a primeira instrução

    while ( fgets(linhaHex, sizeof(linhaHex), hexfile) != NULL ){ //enquanto houver linhas..
        if (strcmp(linhaHex, "<code>\n") == 0){
            flagCode = 1;
            flagData = 0;
            inicioCodigo = indiceEndereco.valor; //apos CODE está a primeira instrução
        }
        else if (strcmp(linhaHex, "<data>\n") == 0){
            flagCode = 0;
            flagData = 1; //There can be only one!
        }
        else {
            if (flagData){ //se está na regiao de dados, guarda todos os 40 bits..
                dado.valor = strtoll(linhaHex, NULL, 16); //aqui tem que ser long long int
                GravarMem(indiceEndereco, dado); //guarda o dado no endereco atual
                indiceEndereco.valor++; //ja aponta para o proximo endereço
            }
            else if (flagCode){ //se está na regiao de instruções, pega duas linhas do arquivo antes de gravar..
                if (flagEsquerda){
                    //Se esta instrução é da esquerda, DADO receberá o valor e será deslocado 20 bits
                    //para a esquerda, atingindo a posição correta e criando 20 zeros no fim do valor.
                    dado.valor = strtoll(linhaHex, NULL, 16);
                    dado.valor = dado.valor << 20;
                    GravarMem(indiceEndereco, dado); //Grava agora pois pode não haver instrução à direita
                    //O INDICEENDERECO continuará o mesmo até que haja uma instrução à direita.
                }
                else {
                    //Se está na direita, ja deve haver algum valor em DADO e há bits livres no fim desse valor
                    //Agora, a instrução da direita será somada nos 20bits livres de DADO
                    //e esse novo valor substituirá o valor gravado anteriormente neste ENDERECO.
                    dado.valor += strtol(linhaHex, NULL, 16);
                    GravarMem(indiceEndereco, dado);
                    indiceEndereco.valor++; //como é instrução da direira, a proxima será gravada em outro endereço
                }
                flagEsquerda = !flagEsquerda; //negação do valor boleano (Flip)
            }
        }
    }

    fclose(hexfile);
    return (inicioCodigo); //Sucesso!!!!
}
