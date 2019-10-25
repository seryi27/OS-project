/**
* @brief En este archivo .h se declara la funcion que simula a los apostadores
* realizando apuestas en la previa a la carrera.
* @file apostador.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef APOSTADOR_H
#define APOSTADOR_H

/**
* @brief funcion que genera un proceso hijo por cada apostador. Cada uno de ellos
* genera de forma aleatoria apuestas a los caballos mientras tengan dinero disponible.
* Una vez que comience la carrera no se aceptaran mas apuestas.
* @param apostadores entero que representa el numero total de apostadores que habra
* en la carrera.
* @param num_caballos entero que representa el numero de caballos que participaran en
* la carrera
* @param dinero_total entero que representa cuanto dinero tiene cada apostador para
* apostar en la carrera.
* @param msgid entero que identifica la cola de mensajes que usara el proceso apostador
* para comunicarse con las ventanillas del gestor de apuestas.
*/
void proceso_apostador(int apostadores, int num_caballos, int dinero_total, int msgid);

/**
* @brief manejador de señales que espera una señal desde el proceso principal para cuando
* la carrera termine. Simplemente mata a todos los procesos apostador (sus hijos) y termina
* con EXIT_SUCCESS
* @param sig entero que identifica la señal que ha forzado la ejecucion de este manejador.
*/
void manejador_terminar(int sig);

#endif
