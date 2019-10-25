/**
* @brief En este archivo .h se declaran todas las funciones relacionadas con la
* creacion de memoria compartida del programa. Dado que utilizamos fuertemente este
* modo de comunicacion, tenemos varias funciones, una para cada estructura de datos
* que compartimos.
* @file memoria.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef MEMORIA_H
#define MEMORIA_H

/**
* @brief funcion principal que genera toda la memoria compartida que el programa
* utiliza, llamando a cada una de las funciones definidas debajo de forma controlada.
* @return 0 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int crear_memoria();

/**
* @brief genera una zona de memoria compartida para un array de doubles que almacenan
* la cotizacion de cada caballo en cada momento.
* @return 0 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int crear_memoria_cotizacion();

/**
* @brief genera la zona de memoria compartida de la informacion de la carrera, que
* almacena la evolucion de la carrera, guardando la distancia, el avance y la posicion de
* cada caballo.
* @return 0 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int crear_memoria_carrera();

/**
* @brief genera la zona de memoria compartida de las flags(dos enteros que permiten
* a todo proceso que tenga acceso a ellas el punto de ejecucion del proceso principal)
* @return 0 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int crear_memoria_flags();

/**
* @brief genera la zona de memoria compartida de las apuestas (una matriz que relaciona
* apostadores y caballos a traves del dinero total que el apostador ha apostado a cada uno)
* @return 0 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int crear_memoria_apuestas();

#endif
