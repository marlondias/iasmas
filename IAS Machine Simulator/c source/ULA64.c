/** Este código vai realizar operações aritméticas e lógicas, utilizando instruçoes em linguagem assembly.

    Atenção: Esta é a versão 64bit!! Este código é baseado em registradores de 64 bits e NÃO executa
    corretamente em máquinas que não possuam registradores 64 bit.

    Operações:
        0: ac + mem
        1: ac + |mem|
        2: ac - mem
        3: ac - |mem|
        4: ac * mem _ MSB em AC, LSB em MQ
        5: ac / mem _ quociente em MQ, resto em AC
        6: ac << 1
        7: ac >> 1
        8: mbr --> mq
        9: mq --> ac
        10: mbr --> ac
        11: ac --> mbr

    Resultado dos testes:
        SHIFTs funcionam!
        ADDs e SUBs funcionam!
        MOVs funcionam!
        MUL funciona lindamente agora!
        DIV agora funciona mas..

**/

///Status: Testada com sucesso / Aparentemente pronta

#include "../headers/stdias.h" //que tambem contem inttypes.h

//Definição dos registradores da ULA
int40_t reg_MBR; //Memory Buffer Register
int40_t reg_AC; //Accumulator
int40_t reg_MQ; //Multiplier Quotient


static int8_t operacao; //8bits
static int64_t regmbr = 0L, regac = 0L, regmq = 0L; //64bits (40 usados)

void ULA64(int8_t numeroOperacao){
    operacao = numeroOperacao;
    regac = reg_AC.valor;
    regmq = reg_MQ.valor;
    regmbr = reg_MBR.valor;

    asm volatile (
        "movq regac, %rax\n\t" //A partir de agora, RAX é AC e RDX é MQ..
        "movq regmq, %rdx\n\t"

        //COMPARAÇÕES
        "movb $0, %cl\n\t" //usando apenas o tamanho necessario do registrador C
        "cmpb operacao, %cl\n\t" //fato que não tem muito impacto na performance.. serve mais como um showoff do que eu aprendi de assembly #sinceridadeUé
        "je soma\n\t" //IF (operacao == 0) {goto soma}

        "movb $1, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je somaabs\n\t" //IF (operacao == 1) {goto somaabs}

        "movb $2, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je subtracao\n\t" //IF (operacao == 2) {goto sub}

        "movb $3, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je subabs\n\t" //IF (operacao == 3) {goto subabs}

        "movb $4, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je multiplicacao\n\t" //IF (operacao == 4) {goto multiplicacao}

        "movb $5, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je divisao\n\t" //IF (operacao == 5) {goto divisao}

        "movb $6, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je lshift\n\t" //IF (operacao == 6) {goto lshift}

        "movb $7, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je rshift\n\t" //IF (operacao == 7) {goto rshift}

        "movb $8, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je mbrparamq\n\t" //IF (operacao == 8) {goto MBRparaMQ}

        "movb $9, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je mqparaac\n\t" //IF (operacao == 9) {goto MQparaAC}

        "movb $10, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je mbrparaac\n\t" //IF (operacao == 10) {goto MBRparaAC}

        "movb $11, %cl\n\t"
        "cmpb operacao, %cl\n\t"
        "je acparambr\n\t" //IF (operacao == 11) {goto ACparaMBR}

        "jmp fim\n\t" //ELSE {goto fim}


        //OPERAÇÔES:
        "soma: addq regmbr, %rax\n\t"
        "jmp fim\n\t"

        "somaabs: cmpq $0, regmbr\n\t" //IF (MBR >= 0) {soma normalmente}
        "jge soma2\n\t"
        "negq regmbr\n\t" //ELSE {soma com -MBR}
        "soma2: addq regmbr, %rax\n\t"
        "jmp fim\n\t"

        "subtracao: subq regmbr, %rax\n\t"
        "jmp fim\n\t"

        "subabs: cmpq $0, regmbr\n\t" //IF (MBR >= 0) {subtrai normalmente}
        "jge subtra2\n\t"
        "negq regmbr\n\t" //ELSE {subtrai com -MBR}
        "subtra2: subq regmbr, %rax\n\t"
        "jmp fim\n\t"

        "multiplicacao: imulq regmbr\n\t" //IMUL pq é signed -- Faz MBR * RAX, coloca o resultado na dupla RDX:RAX
        "xchgq %rax, %rdx\n\t" //No IAS é invertido, msb -> AC, lsb -> MQ
        "jmp fim\n\t"

        "lshift: salq %rax\n\t" //SAL pq é signed
        "jmp fim\n\t"

        "rshift: sarq %rax\n\t"
        "jmp fim\n\t"

        "mbrparamq: movq regmbr, %rdx\n\t" //apenas move MBR -> MQ
        "jmp fim\n\t"

        "mqparaac: movq %rdx, %rax\n\t" //move e zera o MQ
        "jmp fim\n\t"

        "mbrparaac: movq regmbr, %rax\n\t" //apenas move MBR -> AC
        "jmp fim\n\t"

        "acparambr: movq %rax, regmbr\n\t" //apenas move AC -> MBR
        "jmp fim\n\t"


        "divisao: jmp fim\n\t" //Cheatcode ativar!!
        /*
        "divisao: cmpq $0, regmbr\n\t" //Se o divisor for 0, evita um bug épico..
        "je fim\n\t"
        "movq $0, %rdx\n\t" //RDX tinha o valor de MQ, agora tem 0. Portanto, a dupla RDX:RAX será 000000000000:RAX (lembrando que RAX é o AC)
        "cqo\n\t" //sign-extend não sei pra que (viva internet!)
        "idivq regmbr\n\t" //AC é o dividendo, MBR é o divisor. Efetua a divisão, resto em RDX e quociente em RAX
        "xchgq %rax, %rdx\n\t" //No IAS é invertido, quociente -> RDX, resto -> RAX
        "jmp fim\n\t" //Ufa..
        */

        //FINALIZANDO..
        "fim: movq %rax, regac\n\t"
        "movq %rdx, regmq\n\t"
    );

    if (operacao == 4){
        //Se foi MUL, precisa arrumar os bits antes de devolver o valor (lembra que o corte é automático?)
        reg_MQ.valor = regmq; //aqui é normal
        regmq = regmq >> 40; //desloca o que ja foi passado
        regac = regac << 24; //abre um espaço pra colocar o que sobrou de MQ
        reg_AC.valor = regac + regmq; //une 24 bits de MQ com o restante de AC
    }
    else if (operacao == 5){
        //Se for DIV, vai ter que ser pelo C mesmo, por que não dá..
        if (regmbr == 0) return; //Divisão por zero, nada acontece
        reg_MQ.valor = regac / regmbr; //quociente
        reg_AC.valor = regac % regmbr; //resto
        reg_MBR.valor = regmbr;
    }
    else {
        reg_AC.valor = regac; //Devolve normalmente os valores..
        reg_MQ.valor = regmq;
        reg_MBR.valor = regmbr;
    }

    return;
}
