/**
* @brief En este archivo .h se declara la estructura del tipo de mensaje usado
* para la communicacion entre procesos apostador y los hilos ventanilla. Incluyen
* la informacion relevante de una apuesta(quien la realiza, a que caballo y cuanto apuesta)
* @file mensaje.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

#ifndef MENSAJE_H
#define MENSAJE_H

typedef struct _mensaje1{
  long id;
  char nombre[20];
  int caballo;
  int apostador;
  double apuesta;
}mensaje1;


#endif
