/**
* @brief En este archivo .h se declaran varias funciones necesarias para trabajar
*        con semáforos, desde crearlos, inicializarlos, borrarlos y hacer las
*        operaciones básicas del semáforo, como son Up y Down.
* @file semaforos.h
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 02-04-2018
*/

#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#define ERROR -1
#define OK 0

/***************************************************************
  Nombre:
      Inicializar_Semaforo.
  @brief
      Inicializa los semaforos indicados.
  @param
      int semid: Identificador del semaforo.
  @param
      unsigned short *array: Valores iniciales.
  @param
      int size: tamaño del array.
  @return
      int: OK si todo fue correcto,
           ERROR en caso de error.
************************************************************/

int Inicializar_Semaforo(int semid, unsigned short *array, int size);

/***************************************************************
Nombre:
    Borrar_Semaforo.
@brief
    Borra un semaforo.
@param
    int semid: Identificador del semaforo.
@return
    int: OK si todo fue correcto,
         ERROR en caso de error.
***************************************************************/

int Borrar_Semaforo(int semid);

/***************************************************************
Nombre:
    Crear_Semaforo.
@brief
    Crea un semaforo con la clave y el tamaño especificado. Lo inicializa a 0.
@param
    key_t key: Clave precompartida del semaforo.
@param
    int size: Tamaño del semaforo.
@param
    int *semid: Identificador del semaforo.
@return
    int: ERROR en caso de error,
        0 si ha creado el semaforo,
        1 si ya estaba creado.
**************************************************************/

int Crear_Semaforo(key_t key, int size, int *semid);

/**************************************************************
Nombre:
    Down_Semaforo
@brief
    Baja el semaforo indicado
@param
    int semid: Identificador del semaforo.
@param
    int num_sem: Semaforo dentro del array.
@param
    int undo: Flag de modo persistente pese a finalización abrupta.
@return
    int: OK si todo fue correcto,
         ERROR en caso de error.
***************************************************************/

int Down_Semaforo(int id, int num_sem, int undo);

/***************************************************************
Nombre:
    DownMultiple_Semaforo
@brief
    Baja todos los semaforos del array indicado por active.
@param
    int semid: Identificador del semaforo.
@param
    int size: Numero de semaforos del array.
@param
    int undo: Flag de modo persistente pese a finalización abrupta.
@param
    int *active: Semaforos involucrados.
@return
    int: OK si todo fue correcto,
         ERROR en caso de error.
***************************************************************/

int DownMultiple_Semaforo(int id,int size,int undo,int *active);


/**************************************************************
Nombre:
    Up_Semaforo
@brief
    Sube el semaforo indicado
@param
    int semid: Identificador del semaforo.
@param
    int num_sem: Semaforo dentro del array.
@param
    int undo: Flag de modo persistente pese a finalizacion abupta.
@return
    int: OK si todo fue correcto,
         ERROR en caso de error.
***************************************************************/

int Up_Semaforo(int id, int num_sem, int undo);

/***************************************************************
Nombre:
    UpMultiple_Semaforo
@brief
    Sube todos los semaforos del array indicado por active.
@param
    int semid: Identificador del semaforo.
@param
    int size: Numero de semaforos del array.
@param
    int undo: Flag de modo persistente pese a finalización abrupta.
@param
    int *active: Semaforos involucrados.
@return
    int: OK si todo fue correcto,
         ERROR en caso de error.
***************************************************************/

int UpMultiple_Semaforo(int id,int size, int undo, int *active);

#endif //SEMAFOROS_H
