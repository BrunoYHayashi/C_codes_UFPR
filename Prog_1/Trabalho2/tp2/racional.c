/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo de implementação para TAD racional.
 * Feito em 20/09/2024 para a disciplina CI1001 - Programação 1.
 *
 * Este arquivo deve conter as implementações das funções cujos protótipos
 * foram definidos em racional.h. Neste arquivo também podem ser definidas
 * funções auxiliares para facilitar a implementação daquelas funções.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"

/*
 * Implemente aqui as funcoes definidas no racionais.h; caso precise,
 * pode definir aqui funcoes auxiliares adicionais, que devem ser usadas
 * somente neste arquivo.
*/

/* retorna um número aleatório entre min e max, inclusive. */
long aleat (long min, long max)
{
  long random;
  random= min+ (rand() % (max-min+1));
  return(random);
}

/* Máximo Divisor Comum entre a e b      */
/* calcula o MDC pelo método de Euclides */
long mdc (long a, long b)
{
  long maior, menor, AUX;
  if (a>b) { //condição para obter o maior valor e o menor valor entre a e b
    maior= a;
    menor=b;
  }
  else {
    maior=b;
    menor=a;
  }

  while (menor!=0){ //loop para encontrar o mdc => dividendo² (maior) = divisor¹; divisor²= dividendo¹ % divisor¹
    AUX= maior % menor;
    maior= menor;
    menor= AUX;
  }

  return(maior);  //retorna o último "resto" de divisão anterior ao resto 0
}

/* Mínimo Múltiplo Comum entre a e b */
/* mmc = (a * b) / mdc (a, b)        */
long mmc (long a, long b)
{
  long minMulCom;
  minMulCom= ((a*b)/(mdc(a, b))); //utiliza a fórmula para calcular o mínimo múltiplo comum e retorna esse valor
  return(minMulCom);
}

static long verifica_negativo (long a){ //verifica se um número é negativo. Retorna 1 se for e 0 se não for
  if (a<0)
    return(1);
  return(0);
}

static long verifica_sinalDiv (long a, long b){ //verifica o sinal da divisão, utiliza da função verifica_negativo (retorna 1 se o número é negativo), logo, se a soma der 1 (+- || -+) é negativo, caso contrário é positivo. Retorna 1 se o sinal da divisão é negativo e 0 se é positivo
  long numNeg, denNeg;
  numNeg= verifica_negativo(a);
  denNeg= verifica_negativo(b);
  if ((numNeg+denNeg)==1)
    return(1);
  return(0);
}

/* Recebe um número racional e o simplifica.
 * Por exemplo, ao receber 10/8 deve retornar 5/4.
 * Se ambos numerador e denominador forem negativos, deve retornar um positivo.
 * Se o denominador for negativo, o sinal deve migrar para o numerador.
 * Se r for inválido, devolve-o sem simplificar. */
struct racional simplifica_r (struct racional r)
{
  long numSimp= labs(r.num), denSimp=labs(r.den), divisor=mdc(numSimp, denSimp), sinal; //variáveis numSimp e denSimp recebem o valor absoluto de r.num e r.den.
  if (!valido_r(r)) //verifica se r é válido, caso não, retorna ele mesmo
    return(r);

 //simplificação de r. Divide numSimp e denSimp pelo mdc, logo, não é necessário repetições.
  numSimp= numSimp/divisor;
  denSimp= denSimp/divisor;

  sinal= verifica_sinalDiv(r.num, r.den); //verifica o sinal da simplificação

  r.num= numSimp; //atribui o numerador e denominador simplificados ao r.num e r.den
  r.den= denSimp;

  if (sinal==1)
    r.num= ((-1)*r.num);

  return (r); //retorna o r, agora, simplificado
}

/* implemente as demais funções de racional.h aqui */

long numerador_r (struct racional r){
  return(r.num);
}

long denominador_r (struct racional r) {
  return(r.den);
}

struct racional cria_r (long numerador, long denominador){
  struct racional r;
  r.num= numerador; //r.num recebe numerador
  r.den= denominador; //r.den recebe denominador
  return(r);
}

  int valido_r (struct racional r) {
  if (r.den==0)
    return(0); //inválido
  return(1); //válido
}

struct racional sorteia_r (long min, long max){
  long numRand= aleat(min,max), denRand= aleat(min,max); //utiliza a função aleat para criar números aleatórios que ocuparão numRand e denRand
  struct racional r= cria_r(numRand, denRand); //cria o r para ser usado na função, já utilizando numRand e denRand
  r= simplifica_r(r);//simplifica o r
  return(r);
}

void imprime_r (struct racional r){
  r= simplifica_r(r); //simplifica o r

  if (!valido_r(r)){ //verifica se é inválido, se for, imprime inválido e acaba aqui
    printf("NaN ");
    return;
  }

  else if (r.num==0){ //verifica se o numerador é 0, se for, imprime 0 e acaba aqui
    printf("0 ");
    return;
  }

  else if (r.den==1){ //verifica se o denominador é 1, se for, imprime o numerador e acaba aqui
    printf("%ld ", r.num);
    return;
  }

  else if (r.num==r.den){ //verifica se o númerador é igual o denominador, se for, acaba aqui
    printf("1 ");
    return;
  }

  else{
    printf("%ld/%ld ", r.num, r.den); //se não corresponde a nenhuma das condições anteriores, imprime o racional simplificado. Importante ressaltar que o sinal foi resolvido na função simplifica_r
    return;
  }
}

static struct racional resolve_mmc (struct racional r1, long denComum){
  long denr2;

  if (r1.den==denComum)
    return(r1);
  else{
    denr2= denComum/ r1.den;
    r1.num= r1.num* denr2;
    r1.den= denComum;
    return(r1);
  }
}

int compara_r (struct racional r1, struct racional r2){
  if ((!valido_r(r1))||(!valido_r(r2))){
    return (-2);
  }
  else
  {
    long denominadorComum;

    r1=(simplifica_r(r1));
    r2=(simplifica_r(r2));

    denominadorComum= mmc (r1.den, r2.den);

    r1= resolve_mmc(r1, denominadorComum);
    r2= resolve_mmc(r2, denominadorComum);
    if ((r1.num<r2.num))
      return (-1);
    else if ((r1.num>r2.num))
      return(1);
    else
      return(0);
  }
}

int soma_r (struct racional r1, struct racional r2, struct racional *r3){
  struct racional rSoma;

  if (((!valido_r(r1)||(!valido_r(r2))))||(!r3)) {
    return(0);//inválido
  }

  rSoma.den= mmc(r1.den, r2.den); //denominador do r resultado da soma, necessita do mmc dos denominadores dos racionais a serem somados
  rSoma.num= ((r1.num*(rSoma.den/r1.den))+(r2.num*(rSoma.den/r2.den))); //o numerador do r resultado, é a soma dos numeradores dos racionais, cada um multiplicado do mmc dos denominadores
  *r3 = rSoma;
  return(1);
}

int subtrai_r(struct racional r1, struct racional r2, struct racional *r3){
  struct racional rSubt;

  if (((!valido_r(r1)||(!valido_r(r2))))||(!r3)) {
    return(0);//inválido
  }

  rSubt.den= mmc(r1.den, r2.den); //denominador do r resultado da subtração, necessita do mmc dos denominadores dos racionais a serem subtraídos
  rSubt.num= ((r1.num*(rSubt.den/r1.den))-(r2.num*(rSubt.den/r2.den))); //o numerador do r resultado, é a subtração dos numeradores dos racionas,com cada um multiplicado do mmc dos denominadores
  *r3= rSubt;
  return(1);
}

int multiplica_r (struct racional r1, struct racional r2, struct racional *r3){
  struct racional rMult;

  if (((!valido_r(r1)||(!valido_r(r2))))||(!r3)) {
    return(0);//inválido
  }

  rMult.num= (r1.num)*(r2.num); //numerador do r resultado, é o produto dos numeradores dos racionais a serem multiplicados
  rMult.den= (r1.den)*(r2.den); //denominador do r resultado, é o produto dos denominadores dos racionais a serem multiplicados
  *r3= rMult;
  return(1);
}

int divide_r (struct racional r1, struct racional r2, struct racional *r3){
  struct racional rDiv;

  if (((!valido_r(r1)||(!valido_r(r2))))||(!r3)||(r2.num==0)) {
    return(0);//inválido
  }

  rDiv.num= (r1.num)*(r2.den); //numerador do r resultado, é o produto do numerador do 1ºracional com o denominador do 2º racional a ser multiplicado
  rDiv.den= (r1.den)*(r2.num); //denominador do r resultado, é o produto do denominador do 1ºracional com o numerador do 2º racional a ser multiplicado
  *r3= rDiv;
  return(1);
}