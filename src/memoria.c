#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "memoria.h"
#include "estructuras.h"

extern int *active;
extern int num_caballos;
extern int id_zone;
extern int id_zone_carrera;
extern int id_zone_flags;
extern int id_zone_apuestas;

int crear_memoria(){
  if(crear_memoria_cotizacion()==-1){
    printf("Error creando la memoria compartida para la cotizacion\n");
    fflush(stdout);
    free(active);
    return -1;
  }

  if(crear_memoria_carrera()==-1){
    printf("Error creando la memoria compartida para la carrera\n");
    fflush(stdout);
    free(active);
    return -1;
  }

  if(crear_memoria_flags()==-1){//flags para control carrera
    printf("Error creando la memoria compartida para las flags\n");
    fflush(stdout);
    free(active);
    return -1;
  }

  if(crear_memoria_apuestas()==-1){//matriz de apuestas
    printf("Error creando la memoria compartida para las apuestas\n");
    fflush(stdout);
    free(active);
    return -1;
  }
  return 0;
}


int crear_memoria_cotizacion(){

  /* we create the shared memory */
  id_zone = shmget (IPC_PRIVATE, num_caballos*sizeof(double), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if (id_zone == -1) {
    perror("shmget");
    fprintf (stderr, "Error with id_zone \n");
    return -1;
  }
  //printf ("ID zone shared memory: %i\n", id_zone);

  double *cotizacion_caballo = shmat (id_zone, NULL, 0);
    if (cotizacion_caballo == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }
  //printf("puntero cotizacion%p\n", cotizacion_caballo);
  return 0;
}

int crear_memoria_carrera(){
  id_zone_carrera = shmget (IPC_PRIVATE, num_caballos*sizeof(info_carrera), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if (id_zone_carrera == -1) {
    perror("shmget");
    fprintf (stderr, "Error with id_zone \n");
    return -1;
  }
  //printf ("ID zone shared memory: %i\n", id_zone_carrera);

  info_carrera *info = shmat (id_zone_carrera, NULL, 0);
    if (info == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }
  //printf("puntero info%p\n", info);

  return 0;
}

int crear_memoria_flags(){
  id_zone_flags = shmget (IPC_PRIVATE, sizeof(flags_carrera), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if (id_zone_flags == -1) {
    perror("shmget");
    fprintf (stderr, "Error with id_zone_flags \n");
    return -1;
  }
  //printf ("ID zone shared memory: %i\n", id_zone_flags);

  flags_carrera *flags = shmat (id_zone_flags, NULL, 0);
    if (flags == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }
  //printf("puntero variable%p\n", flags);

  return 0;
}

int crear_memoria_apuestas(){
  id_zone_apuestas = shmget (IPC_PRIVATE, sizeof(info_apuestas), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if (id_zone_apuestas == -1) {
    perror("shmget");
    fprintf (stderr, "Error with id_zone_apuestas \n");
    return -1;
  }
  //printf ("ID zone shared memory: %i\n", id_zone_apuestas);

  info_apuestas *apuestas = shmat (id_zone_apuestas, NULL, 0);
    if (apuestas == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }
  //printf("puntero variable%p\n", apuestas);

  return 0;
}
