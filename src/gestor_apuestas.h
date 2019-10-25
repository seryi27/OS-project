/**
* @brief En este archivo .h se declaran todas las funciones que tienen que ver con
* gestionar las apuestas llegadas desde los procesos apostador. Incluyen el proceso_gestor_apuestas(),
* que es el más general y que basicamente añade las zonas de memoria compartida y genera hilos
* ventanilla para que gestionen las apuestas, el hilo_ventanilla(), que recibe mensajes de
* apuestas y realiza las operaciones neccesarias para almacenar esa informacion y para
* actualizar las cotizacciones de los caballos, y la funcion guardar_apuesta, que almacena
* en un archivo .txt un historial con todas las apuestas realizadas para que luego se
* imprima esa informacion mediante el proceso monitor al finalizar la carrera.
* @file gestor_apuestas.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef GESTOR_APUESTAS_H
#define GESTOR_APUESTAS_H

/**
* @brief proceso que se encarga de tratar las apuestas recibidas. Para ello crea
* n_vent hilos que reciben mensajes desde apostador y tratan con dicha informacion,
* almacenando en un archivo historial cada una de las apuestas con toda la informacion
* relevante, actualizando la cotizacion de los caballos y almacenando en una matriz
* apostador cuanto ganara cada uno de los apostadores si gana cada uno de los caballos.
* Una vez comenzada la carrera deja de recibir apuestas y envia a monitor la informacion
* que ha ido almacenando para que, una vez finalizada la carrera, la muestre por pantalla.
* @param n_cab entero que representa el numero de caballos que disputan la carrera.
* @param n_vent entero que representa el numero de ventanillas que existen para atender a las
* apuestas generadas por los procesos apostador
* @param n_apost entero que representa el numero de apostadores existentes para esta carrera
* @param msgid entero que identifica la cola de mensajes usada por los hilos ventanilla
*/
void proceso_gestor_apuestas(int n_cab, int n_vent, int n_apost, int msgid);

/**
* @param arg puntero a void que contiene un puntero a una estructura argumentos.
* Esta estructura guarda todos los parametros necesarios para la correcta ejecucion
* del hilo. Ver estructuras.h para mas informacion
* @return es vacio puesto que aunque sea necesario indicar void * (por ser funcion para hilo)
* no queremos devolver nada
*/
void *hilo_ventanilla(void *arg);

/**
* @brief esta funcion se encarga de almacenar en un archivo historial cada una de
* las apuestas realizadas en los momentos previos a la ejecucion de la carrera.
* Esta informacion sera usada al final por el monitor.
* @param apostador el apostador que ha realizado dicha apuesta
* @param ventanilla la ventanilla que ha procesado la apuesta
* @param caballo caballo al que se ha apostado
* @param cotizacion_caballo la cotizacion del caballo en el momento de la apuesta
* @param apuesta cantidad de dinero apostado
* @return 1 o -1 en funcion de si ejecuta correcta o incorrectamente
*/
int guardar_apuesta(int apostador, int ventanilla, int caballo, double cotizacion_caballo, double apuesta);

void manejador_gestor_terminar(int sig);

#endif
