/**
* @brief En este archivo .h se declaran estructuras varias usadas a lo largo del
* programa de forma recurrente. Incluye info_carrera, que se utiliza para, argumentos,
* que se utiliza para crear los hilos ventanilla y enviarles la informacion necesaria
* para su correcta ejecucion y flags_carrera, cuyas variables permiten a todos los
* procesos que lo requieran conocer el punto de ejecucion de la carrera (momentos anteriores
* a la carrera, durante la ejecucion de la carrera o una vez finalizada).
* @file estructuras.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/*estructura que guarda la informacion de la carrera*/
typedef struct{
  int distancia;
  int avance;
  int posiciones;
}info_carrera;

/*estructura que guarda las flags de la carrera*/
typedef struct{
  int carrera_iniciada;
  int carrera_terminada;
}flags_carrera;

/*
 * Estructura que guarda el dinero que recibe cada apostador en caso de que
 * gane el caballo apostado.
 */
typedef struct{
  double matriz[100][10];//Maximo 100 apostadores y 10 caballos
  int ganadores[10];
  double restante[100];//dinero con el que ha concluido la previa de la carrera cada apostador
}info_apuestas;

/* estructura que recibe un hilo ventanilla  usado en gestor_apuestas*/
typedef struct _argumentos{
  int msgid;
  double * cotizacion_caballo; // array con la cotizacion de cada caballo
  info_apuestas *apuestas; // matriz que relaciona cada apostador con el dinero apostado a cada caballo
  double *total_apostado; // total de dinero apostado en la carrera (suma de las apuestas a cada caballo)
  double *caballos; // array con el total de dinero apostado a cada caballo
  int *ventanilla; // identifica cada ventanilla de forma unica
  flags_carrera *flags; // frag que cambia de valor al terminar el tiempo de apuestas para avisar de que comienza la carrera
}argumentos;




#endif
