#include <stdlib.h>
#include "environment.h"

int aleat (int min, int max){
    int nAleat;

    nAleat= rand() % (max-min +1); //max-min = N. Rand pega de 0 a N-1, por isso "+1"
    nAleat= min+ nAleat; //Desloca +min para com certeza alcançar o número mínimo

    return nAleat;
}