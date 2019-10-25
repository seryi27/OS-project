/**
* @brief En este archivo .h se declara la funcion aleat_num(), que usaremos cada vez
* que necesitemos obtener algun numero aleatorio.
* @file aleat_num.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 02-04-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef ALEAT_NUM_H
#define ALEAT_NUM_H

/**
* @brief funcion que dados dos enteros genera un entero aleatorio entre estas
* cotas
* @param inf entero que representa la cota inferior para el entero aleatorio a
* generar
* @param sup entero que representa la cota superior para el entero aleatorio a
* generar
* @return devuelve el entero generado aleatoriamente.
*/
int aleat_num(int inf, int sup);

#endif
