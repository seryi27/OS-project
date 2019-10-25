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
#include "monitor.h"
#include "semaforos.h"
#define PATH "historial.txt"



/**
* funcion auxiliar de ordenar_caballos(). Recibe dos punteros a resultado_caballo
* e intercambia su contenido
* @param a resultado_caballo 1
* @param b resultado_caballo 2
*/
void swap1(resultado_caballo *a, resultado_caballo *b);

/**
* funcion auxiliar de ordenar_caballos(). Recibe dos punteros a ganadores
* e intercambia su contenido
* @param a ganadores 1
* @param b ganadores 2
*/
void swap2(ganadores *a, ganadores *b);



extern int sem_id_caballos;
extern int sem_id_info_carrera;
extern int num_caballos;
extern int num_apostadores;
extern int id_zone;
extern int id_zone_carrera;
extern int id_zone_flags;
extern int id_zone_apuestas;
extern int dinero;

void proceso_monitor(){
  flags_carrera *flags;

  monitor_previa();

  flags = shmat (id_zone_flags, NULL, 0);
    if (flags == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }



  /* Esperamos por SIGUSR2 durante la carrera, pues, junto a un par de semaforos,
   * es la señal usada para coordinar la impresion de la evolucion de la carrera
   * con el desarrollo de la misma (para cada ronda se recibe una señal para que se imprima
   * la situacion actual)
   */
  signal(SIGUSR2, monitor_carrera);
  signal(SIGINT, report_completo);

  /* mientras se desarrolle la carrera */
  while(flags->carrera_terminada==0){
    pause();
  }

  monitor_postcarrera();

  sleep(15);
  report_completo(0);

  exit(EXIT_SUCCESS);
}

int monitor_previa(){
  double *cotizacion_caballo;

  cotizacion_caballo = shmat (id_zone, NULL, 0);
    if (cotizacion_caballo == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }
  /* Imprimimos cada segundo tanto el tiempo restante como la cotizacion de cada
   * caballo.
   */
  for(int i=30;i>0;i--){
    printf("Segundos restantes para comenzar la carrera: %d\n", i);
    fflush(stdout);
    for(int j=0; j<num_caballos;j++){
      Down_Semaforo(sem_id_caballos, j, SEM_UNDO);
      printf("Cotizacion del caballo %d: %lf\n", j,cotizacion_caballo[j]);
      Up_Semaforo(sem_id_caballos, j, SEM_UNDO);
    }
    printf("\n");
    sleep(1);
  }

  /* despertamos al proceso principal para que comience la carrera */
  kill(getppid(),SIGALRM);

  return 0;
}

void monitor_carrera(int sig){
  if(sig != SIGUSR2){
    return;
  }

  info_carrera *info;

  info = shmat (id_zone_carrera, NULL, 0);
    if (info == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  /* Imprimimos la situacion actual de cada caballo */
  for(int i=0;i<num_caballos;i++){
    Down_Semaforo(sem_id_info_carrera, i, SEM_UNDO);
    printf("El caballo %d lleva recorridos %d metros. Ha avanzado en esta ronda %d metros\n", i, info[i].distancia, info[i].avance);
    Up_Semaforo(sem_id_info_carrera, i, SEM_UNDO);
  }
  printf("\n");
  fflush(stdout);
  //kill(getppid(), )
  kill(getppid(),SIGALRM);
}

void monitor_postcarrera(){
  info_apuestas *apuestas;
  info_carrera *carrera;
  resultado_caballo resultado[num_caballos];
  ganadores apostadores_ganadores[num_apostadores];
  int k = 0;
  int distancia = 0;


  carrera = shmat (id_zone_carrera, NULL, 0);
    if (carrera == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  apuestas = shmat (id_zone_apuestas, NULL, 0);
    if (apuestas == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  ordenar_caballos(carrera, resultado);

  for(int i = num_caballos-1, j=1; i >= 0; i--, j++){
    if(distancia != resultado[i].distancia){
      printf("El caballo %d ha quedado el número %d, habiendo recorrido %d metros\n", resultado[i].caballo, j, resultado[i].distancia);
      k = 0;
    }
    else{
      k++;
      printf("El caballo %d ha quedado el número %d, habiendo recorrido %d metros\n", resultado[i].caballo, j-k, resultado[i].distancia);
    }
    distancia = resultado[i].distancia;
  }

  printf("\n\n");

  ordenar_apostadores(apuestas, apostadores_ganadores);
  if(num_apostadores>10){
    for(int i = num_apostadores-1; i>= num_apostadores-11; i--){
      printf("El Apostador-%d ha ganado %lf euros con esta carrera\n", apostadores_ganadores[i].apostador, apostadores_ganadores[i].ganancias);
    }
  }else{
    for(int i = num_apostadores-1; i>= 0; i--){
      printf("El Apostador-%d ha ganado %lf euros con esta carrera\n", apostadores_ganadores[i].apostador, apostadores_ganadores[i].ganancias);
    }
  }

}

void report_completo(int sig){
  FILE *fp;
  int apostador, ventanilla, caballo;
  double cotizacion_caballo, apuesta;
  int i = 1;
  resultado_caballo resultado[num_caballos];
  ganadores apostadores_ganadores[num_apostadores];
  info_apuestas *apuestas;
  info_carrera *carrera;
  int distancia = 0;
  int k = 0;

  carrera = shmat (id_zone_carrera, NULL, 0);
    if (carrera == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  apuestas = shmat (id_zone_apuestas, NULL, 0);
    if (apuestas == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }

  ordenar_caballos(carrera, resultado);

  fp = fopen(PATH, "r");
  if(!fp){
    perror("monitor_postcarrera ... error al leer archivo");
  }
  /*imprimimos el historial de apuestas realizadas antes de la carrera */
  printf("Historial de apuestas:\n");
  while(!feof(fp)){
    fscanf(fp, "%d %d %d %lf %lf\n", &apostador, &ventanilla, &caballo, &cotizacion_caballo, &apuesta);
    printf("%d-El apostador %d apostó en la ventanilla %d una cantidad de %lf euros al caballo %d cuando su cotizacion era de %lf\n", i, apostador, ventanilla, apuesta, caballo, cotizacion_caballo);
    i++;
  }

  printf("\n\n");
  printf("Resultado de la carrera:\n");
  for(int i = num_caballos-1, j=1; i >= 0; i--, j++){
    if(distancia != resultado[i].distancia){
      printf("El caballo %d ha quedado el número %d, habiendo recorrido %d metros\n", resultado[i].caballo, j, resultado[i].distancia);
      k = 0;
    }
    else{
      k++;
      printf("El caballo %d ha quedado el número %d, habiendo recorrido %d metros\n", resultado[i].caballo, j-k, resultado[i].distancia);
    }
    distancia = resultado[i].distancia;
  }

  printf("\n\n");

  informacion_apostadores(apuestas, apostadores_ganadores);

  printf("Resultado de los apostadores:\n");
  for(int i = 0; i < num_apostadores; i++){
    printf("El Apostador-%d ha apostado %lf durante la previa de la carrera. Con estas apuestas ha obtenido un beneficio de %lf, por lo que le queda %lf\n",
     i, (double) dinero - apuestas->restante[i], apostadores_ganadores[i].ganancias, apuestas->restante[i] + apostadores_ganadores[i].ganancias);
  }


  exit(EXIT_SUCCESS);
}

void ordenar_caballos(info_carrera *carrera, resultado_caballo *resultado){
  int i=0, j=0;

  /* inicializamos resultado con los valores de carrera */
  for(int i = 0; i < num_caballos; i++){
    resultado[i].caballo = i;
    resultado[i].distancia = carrera[i].distancia;
  }

  /* realizamos la ordenacion */
  for(i=num_caballos-1;i>0;i--){
    j=0;
    for(j=0; j<i; j++){
      if(resultado[j].distancia > resultado[j+1].distancia){
        swap1(&resultado[j], &resultado[j+1]);
      }
    }
  }
}

void ordenar_apostadores(info_apuestas *apuestas, ganadores *apostadores_ganadores){
  int i=0, j=0;

  /* inicializamos apostadores con los valores  */
  for(int i = 0; i < num_apostadores; i++){
    apostadores_ganadores[i].apostador = i;
    apostadores_ganadores[i].ganancias = 0;
    for(int j = 0; j < num_caballos; j++){
      if(apuestas->ganadores[j] == 1){
        apostadores_ganadores[i].ganancias += apuestas->matriz[i][j];
      }
    }
  }

  /* realizamos la ordenacion */
  for(i=num_apostadores-1;i>0;i--){
    j=0;
    for(j=0; j<i; j++){
      if(apostadores_ganadores[j].ganancias > apostadores_ganadores[j+1].ganancias){
        swap2(&apostadores_ganadores[j], &apostadores_ganadores[j+1]);
      }
    }
  }
}

void informacion_apostadores(info_apuestas *apuestas, ganadores *apostadores_ganadores){
  for(int i = 0; i < num_apostadores; i++){
    apostadores_ganadores[i].apostador = i;
    apostadores_ganadores[i].ganancias = 0;
    for(int j = 0; j < num_caballos; j++){
      if(apuestas->ganadores[j] == 1){
        apostadores_ganadores[i].ganancias += apuestas->matriz[i][j];
      }
    }
  }
}

void swap1(resultado_caballo *a, resultado_caballo *b){
  resultado_caballo aux;

  if(!a || !b){
    return;
  }

  aux=*a;
  *a=*b;
  *b=aux;

  return;
}

void swap2(ganadores *a, ganadores *b){
  ganadores aux;

  if(!a || !b){
    return;
  }

  aux=*a;
  *a=*b;
  *b=aux;

  return;
}
