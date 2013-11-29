/** StdIAS é uma biblioteca dos tipos comuns usados pelos modulos do simulador.
    Estes tipos foram criados para que as transferências de dados sejam mais
    naturais e parecidas com o que ocorreria realmente em um computador IAS.

    O nome "StdIAS" foi escolhido pois a princípio várias operações seriam comuns entre
    os módulos do IAS e estas operações estariam aqui, mas isto acabou sendo descartado. **/

///Status: Pronto

#ifndef STDIAS_H_INCLUDED //Guardas de inclusão, evitam a confusão!
#define STDIAS_H_INCLUDED

#include <inttypes.h>

typedef struct {
    int64_t valor : 40;  //Signed int de 64 bits, limitado a 40 bits (1 sinal + 39 valor)
} int40_t;

typedef struct {
    uint32_t valor : 20;  //Unsigned int de 32 bits, limitado a 20 bits (20 de valor)
} uint20_t;

typedef struct {
    uint16_t valor : 12;  //Unsigned int de 32 bits, limitado a 12 bits (12 de valor)
} uint12_t;

#endif // STDIAS_H_INCLUDED
