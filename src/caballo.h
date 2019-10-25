/**
* @brief En este archivo .h se declaran tanto la enumeracion de posiciones de los caballos
* como las funciones que tienen que ver con los mismos. Esta es la funcion caballo_tira_dado()
* que genera una tirada aleatoria en funcion de la posicion.
* @file caballo.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef CABALLO_H
#define CABALLO_H


enum posicion {ULTIMO, MEDIO, PRIMERO};


/**
* @brief funcion que dada una posicion de un caballo genera aleatoriamente una
* tirada que representa cuanto avanza el caballo en dicha ronda. Esta tirada depende
* de la posicion del caballo en dicho turno.
* @param recibe un entero que representa la posicion en la que se encuentra el
* caballo. Este entero puede ser 0,1 o 2, representando cada uno ULTIMO, MEDIO o
* PRIMERO.
* @return entero que representa cuanto avanzara el caballo en dicha ronda. Es un
* entero aleatorio entre 1 y 6 si va en medio, entre 1 y 7 si va primero
* y entre 2 y 12 si va ultimo
*/
int caballo_tira_dado(int);
/*
 * El caballo avanza diferente si se encuentra en primera posicion, ultima o
 * entre medias.
 */

#endif //CABALLO_H
