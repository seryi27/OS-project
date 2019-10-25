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

#include "estructuras.h"
#include "gestor_apuestas.h"
#include "mensaje.h"
#include "semaforos.h"

#define PATH "historial.txt"


/*Variables globales*/

extern int sem_id_caballos;
extern int sem_id_mutex_apuesta;
extern int id_zone;
extern int id_zone_apuestas;
extern int id_zone_flags;
extern int num_caballos;

void proceso_gestor_apuestas(int num_caballos, int num_ventanillas, int num_apostadores, int msgid){
  pthread_t *hilo;

  double *total_apostado;
  double *caballos; //dinero apostado a cada caballo
  double *cotizacion_caballo; //cotizacion de cada caballo
  //double **apostadores; //dinero a recibir por cada apostador
  info_apuestas *apuestas;
  argumentos arguments[num_ventanillas];
  int ventanilla[num_ventanillas];
  flags_carrera *flags;

  signal(SIGUSR2, manejador_gestor_terminar);

  total_apostado = (double *)calloc(1,sizeof(double));

  caballos=(double *)malloc(num_caballos*sizeof(double));
  if(!caballos){
    perror("malloc error");
    return;
  }

  // cotizacion_caballo=(double*)malloc(num_caballos*sizeof(double));
  //
  // if(!cotizacion_caballo){
  //   perror("malloc error");
  //   free(caballos);
  //   return;
  // }

  hilo=(pthread_t *)malloc(num_ventanillas*sizeof(pthread_t));
  if(!hilo){
    perror("malloc error");
    free(caballos);
    //free(cotizacion_caballo);
    return;
  }

  // apostadores=(double **)malloc(num_apostadores*sizeof(double));
  // if(!apostadores){
  //   perror("malloc error");
  //   free(caballos);
  //   //free(cotizacion_caballo);
  //   free(hilo);
  //   return;
  // }
  // for(int i=0;i<num_apostadores;i++){
  //   apostadores[i]=(double *)calloc(num_caballos,sizeof(double));
  //   if(!apostadores[i]){
  //     perror("malloc error");
  //     for(int j=0;j<i;j++){
  //       free(apostadores[j]);
  //     }
  //     free(apostadores);
  //     free(caballos);
  //     //free(cotizacion_caballo);
  //     free(hilo);
  //     return;
  //   }
  // }

  cotizacion_caballo = shmat (id_zone, NULL, 0);
  if (cotizacion_caballo == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  apuestas = shmat (id_zone_apuestas, NULL, 0);
  if (apuestas == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  flags = shmat (id_zone_flags, NULL, 0);
  if (flags == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  for(int i=0;i<num_apostadores;i++){
    for(int j=0;j<num_caballos;j++){
      apuestas->matriz[i][j]=0;
    }
  }

  for(int i=0;i<num_caballos;i++){
    caballos[i]=1.0;
    *(total_apostado)+=caballos[i];
  }

  for(int i=0;i<num_caballos;i++){
    cotizacion_caballo[i]=(*(total_apostado)/caballos[i]);
    // printf("\tcotizacion_caballo[%d]: %lf\n", i, cotizacion_caballo[i]);
    // fflush(stdout);
  }

  for(int i=0;i<num_ventanillas;i++){
    ventanilla[i] = i;
    arguments[i].msgid = msgid;
    arguments[i].cotizacion_caballo = cotizacion_caballo;
    arguments[i].apuestas = apuestas;
    arguments[i].total_apostado = total_apostado;
    arguments[i].caballos = caballos;
    arguments[i].flags = flags;
    arguments[i].ventanilla = &ventanilla[i];
    pthread_create(&hilo[i],NULL, &hilo_ventanilla, (void *)&arguments[i]);
  }

  for(int i=0;i<num_ventanillas;i++){
    pthread_join(hilo[i],NULL);
  }

  exit(EXIT_SUCCESS);
}

void *hilo_ventanilla(void *arg){
  argumentos* arguments;
  arguments = (argumentos *)arg;
  mensaje1 msg;
  flags_carrera* flags = arguments->flags;

  /* recibimos mensajes de apuestas mientras flag sea 1 (mientras no acabe el tiempo de apuestas) */
  do{
    msgrcv(arguments->msgid, (struct msgbuf *)&msg, sizeof(mensaje1)-sizeof(long), 2, 0);
    // printf("cantidad apostada: %lf, apostador: %d, caballo: %d\n", msg.apuesta, msg.apostador, msg.caballo);
    if(msg.apuesta != 0){

      /* asignamos al apostador el dinero que ganara si gana el caballo */

      //Down_Semaforo(sem_id_caballos, msg.caballo, SEM_UNDO);
      Down_Semaforo(sem_id_mutex_apuesta, 0, SEM_UNDO);

      arguments->apuestas->matriz[msg.apostador][msg.caballo] += msg.apuesta*arguments->cotizacion_caballo[msg.caballo];
      arguments->caballos[msg.caballo] += msg.apuesta;
      *(arguments->total_apostado) += msg.apuesta;
      //printf("Apuesta al caballo %d: %lf\n", msg.caballo, msg.apuesta);

      /* escribimos una linea en el fichero historial con esta apuesta */
      guardar_apuesta(msg.apostador, *(arguments->ventanilla), msg.caballo, arguments->cotizacion_caballo[msg.caballo], msg.apuesta);

      // printf("\tcotizacion_caballo[%d]: %lf\n", msg.caballo, arguments->cotizacion_caballo[msg.caballo]);
      // fflush(stdout);

      /* actualizamos la cotizacion de los caballos */
      for(int i = 0; i < num_caballos; i++){
        arguments->cotizacion_caballo[i] = *(arguments->total_apostado)/arguments->caballos[i];
        //printf("Cotizacion desde gestor de apuestas: %lf\n", arguments->cotizacion_caballo[i]);
      }

      Up_Semaforo(sem_id_mutex_apuesta, 0, SEM_UNDO);
      //Up_Semaforo(sem_id_caballos, msg.caballo, SEM_UNDO);

    }

  }while((flags->carrera_iniciada)==0);

  pthread_exit(NULL);
}


int guardar_apuesta(int apostador, int ventanilla, int caballo, double cotizacion_caballo, double apuesta){
  FILE *fichero = NULL;

  fichero = fopen(PATH, "a");
  if(!fichero){
    perror("error abriendo archivo\n");
    return -1;
  }
  /* Escrbimos una linea en el historial de apuestas */
  fprintf(fichero, "%d %d %d %lf %lf\n", apostador, ventanilla, caballo, cotizacion_caballo, apuesta);

  fclose(fichero);
  return 1;
}

void manejador_gestor_terminar(int sig){
  if(sig!=SIGUSR2){
    return;
  }
  //printf("Señal SIGUSR2 recibida por el gestor de apuestas\n");
  exit(EXIT_SUCCESS);
}
