#include "caballo.h"
#include "aleat_num.h"

int caballo_tira_dado(int posicion){
  if(posicion!=PRIMERO && posicion!=MEDIO && posicion!=ULTIMO){
    fprintf(stderr, "caballo_tira_dado argumentos invalidos\n");
    return -1;
  }
  /* en funcion de la posicion que recibimos generamos un numero aleatorio de modo distinto */
  if(posicion==PRIMERO){
    return aleat_num(1,7);
  }else if(posicion==MEDIO){
    return aleat_num(1,6);
  }else{ //posicion==ULTIMO
    return aleat_num(1,6)+aleat_num(1,6);
  }
}
