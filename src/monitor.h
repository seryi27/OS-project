/**
* @brief En este archivo .h se declaran todas las funciones relacionadas con el
* monitor de la carrera. Incluye una funcion principal proceso_monitor() que organiza
* temporalmente las llamadas a las otras funciones de monitor. Tambien incluye una
* funcion para cada etapa de la carrera (monitor_previa(), que imprime el tiempo restante
* y las cotizaciones actuales de los caballos, monitor_carrera(), que va imprimiendo
* las posiciones de cada caballo en cada ronda de la carrera y monitor_postcarrera(),
* que muestra las posiciones finales de los caballos y los apostadores que mas han ganado).
* Finalmente incluye report_completo(), una funcion llamada antes de terminar el programa
* para mostrar por pantalla un informe completo de toda la informacion relevante de la carrera.
* @file monitor.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 02-04-2018
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef MONITOR_H
#define MONITOR_H


/* estructura usada por monitor para imprimir el orden de terminacion de los caballos. */
typedef struct{
  int caballo; //identificador del caballo
  int distancia; //distancia recorrida por dicho caballo
}resultado_caballo;

typedef struct{
  int apostador; //identificador del apostador
  double ganancias; //cuanto ha ganado con la carrera
}ganadores;

/**
* @brief funcion principal del monitor. Simplemente ejecuta por orden las funciones siguientes.
* Comienza ejecutando monitor_previa() hasta que comienza la carrera. Durante la carrera espera
* por las señales que recibe de carrera para ir imprimiendo las posiciones de los caballos a traves
* de la funcion manejador monitor_carrera(). Una vez finalizada muestra los resultados de la
* carrera con monitor_postcarrera() y, finalmente, justo antes de acabar utiliza report_completo()
* para mostrar toda la informacion de la carrera de forma detallada.
*/
void proceso_monitor();

/**
* @brief Utiliza memoria recibida para acceder a las cotizaciones de cada caballo y las imprime
* cada segundo junto con un contador del tiempo restante para empezar.
*/
int monitor_previa();

/**
* @brief funcion manejador de señales que es ejecutada al recibir señales desde el proceso
* carrera. De forma organizada gracias a usar señales imprime en cada ronda la distancia
* total recorrida por cada caballo y la distancia recorrida en dicha ronda.
* @param sig entero que representa la señal recibida que activa dicho manejador.
*/
void monitor_carrera(int sig);
/**
* @brief Muestra los resultados de la carrera y de las apuestas (los caballos
* ordenados por posicion final y los 10 apostadores que mas dinero han ganado).
* Se ejecuta una vez finalizada la carrera.
*/
void monitor_postcarrera();

/**
* @brief funcion manejador que se ejecuta justo antes de terminar la ejecucion del
* programa y que imprime un informe completo de la carrera. Este informe incluye
* un historial de todas las apuestas por orden de llegada al sistema, el resultado
* de la carrera (los caballos organizados por distancia recorrida en orden descendente)
* y el resultado de las apuestas de cada apostador.
* @param sig entero que indica la señal que ha producido la ejecucion de este manejador
*/
void report_completo(int sig);

/**
* @brief funcion que devuelve en resultado el orden de finalizacion de los caballos en orden
* descendente (el primero se encuentra al final del array).
* @param carrera array de estructuras info_carrera que usaremos para acceder a la distancia
* recorrida por cada caballo
* @param resultado array de estructuras resultado_caballo (que guarda cada caballo junto con
* la distancia total) que se devolvera ordenado por distancia.
* que ha recorrido. Sera
*/
void ordenar_caballos(info_carrera *carrera, resultado_caballo *resultado);

/**
* @brief funcion que utilizando la memoria compartida apuestas guarda en
* apostadores_ganadores los apostadores ordenados por ganancias en orden ascendente
* (los que mas ganan van los ultimos)
* @param apuestas array de estructuras a info_apuestas que usaremos para acceder a la
* matriz de apuestas y al array de ganadores
* @param apostadores_ganadores array de estructuras a ganadores donde guardamos los
* apostadores ordenados por cuanto dinero han ganado.
*/
void ordenar_apostadores(info_apuestas *apuestas, ganadores *apostadores_ganadores);

void informacion_apostadores(info_apuestas *apuestas, ganadores *apostadores_ganadores);

#endif
