/**
* @brief Este es el archivo principal del programa. Contiene el main y las funciones
* principales de la ejecucion del mismo. Organiza las ejecuciones de los diversos
* procesos que actuan en la ejecucion del programa, ejecuta el proceso principal
* (la carrera propiamente dicha) y controla la terminacion de todos los procesos para
* que el programa termine de forma controlada.
* @file carrera.c
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 05-05-2018
*/

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
#include "assert.h"
#include "caballo.h"
#include "aleat_num.h"
#include "semaforos.h"
#include "monitor.h"
#include "gestor_apuestas.h"
#include "mensaje.h"
#include "memoria.h"
#include "apostador.h"

#define FILEKEY "/bin/cat"
#define KEY 1304
#define PATH "historial.txt"

void fin_carrera(int sig);

void fin_programa(int sig);

void inicio_carrera(int sig);

/**
* @brief La funcion ronda_caballo() simula una ronda de la carrera, generando una
* tirada y comunicandose con el proceso principal para que registre dicho avance y
* actualice las posiciones.
* @param ronda entero que indica cuantas rondas ha habido hasta ahora
* @param pipe_escritura pipe con la que manda informacion al proceso principal
* @param pipe_lectura pipe con la que recibe informacion del proceso principal
* @param num_semaforo identificador del semaforo usado para coordinarse con el p. principal
*/
void ronda_caballo(int ronda, int pipe_escritura, int pipe_lectura, int num_semaforo);


//void proceso_apostador(int apostadores, int num_caballos, int dinero_total, int msgid);

/**
* @brief la funcion ronda_principal() simula una ronda del controlador de la carrera,
* recibiendo las tiradas de los caballos, actualizando sus posiciones y enviandoles de
* nuevo su posicion para la siguiente ronda.
* @param num_caballos entero con el numero de caballos que participan en la carrera
* @param ronda entero que indica cuantas rondas ha habido hasta ahora
* @param pipe_lectura array de pipes usadas para recibir informacion de los hijos
* @param pipe_escritura array de pipes usadas para enviar informacion a los hijos
* @param distancia_recorrida array de enteros donde se almacena la distancia recorrida por cada caballo
* @param long_carrera entero que indica la distancia que requiere recorrer un caballo para ganar
* @return int con la flag carrera terminada. (0 mientras este en curso y 1 al terminar)
*/
int ronda_principal(int num_caballos, int ronda, int **pipe_lectura, int **pipe_escritura, int *distancia_recorrida, int long_carrera);

/**
* @brief funcion que dado el array de distancias recorridas almacena en posiciones
* ULTIMO MEDIO o PRIMERO para indicar a cada caballo en que parte de la carrera se encuentra
* @param distancia_recorrida array de enteros que contiene la distancia recorrida por cada caballo
* @param posiciones array de entero donde se almacena la posicion que ocupa cada caballo en la carrera
* @param num_caballos entero que indica cuantos caballos disputan la carrera
*/
void get_posiciones(int *distancia_recorrida, int *posiciones, int num_caballos);

int crear_semaforos(int num_caballos);

// int crear_memoria_cotizacion();
//
// int crear_memoria_carrera();
//
// int crear_memoria_flags();
//
// int crear_memoria_apuestas();

int sem_id_mutex1;
int sem_id_mutex2;
int sem_id_caballos;
int sem_id_info_carrera;
int sem_id_mutex_apuesta;

int *active;
int num_caballos;
int num_apostadores;
int id_zone;
int id_zone_carrera;
int id_zone_flags;
int id_zone_apuestas;
int pid_caballos[10];
int pid_monitor;
int pid_apostador;
int pid_gestor;
int flag_carrera = 1;
flags_carrera *flags;
int dinero;

int main(int argc, char *argv[]){
  int long_carrera;
  int num_ventanillas;

  int k;//variable para la creacion y manejo de tuberias de cada proceso
  int pid;
  int msgid;
  int status;
  int identificador;
  int pipe_status;
  int **pipes_escritura_principal;
  int **pipes_lectura_principal;


  /*
   * Control de argumentos de entrada
   */

  if(argc<6){
    fprintf(stderr, "Parametros insuficientes\n");
    fprintf(stderr, "\targv[1]: numero de caballos < 10\n");
    fprintf(stderr, "\targv[2]: longitud de la carrera\n");
    fprintf(stderr, "\targv[3]: numero de apostadores < 100\n");
    fprintf(stderr, "\targv[4]: numero de ventanillas\n");
    fprintf(stderr, "\targv[5]: dinero por apostante\n");

    exit(EXIT_FAILURE);
  }
  if(atoi(argv[1])<1){
    fprintf(stderr, "Parametros invalidos: argv[1]=%d\n", atoi(argv[1]));
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[2])<1){
    fprintf(stderr, "Parametros invalidos: argv[2]=%d\n", atoi(argv[2]));
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[3])<1){
    fprintf(stderr, "Parametros invalidos: argv[3]=%d\n", atoi(argv[3]));
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[4])<1){
    fprintf(stderr, "Parametros invalidos: argv[4]=%d\n", atoi(argv[4]));
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[5])<1){
    fprintf(stderr, "Parametros invalidos: argv[5]=%d\n", atoi(argv[5]));
    exit(EXIT_FAILURE);
  }

  /*
   * Inicializacion de variables
   */

  if(atoi(argv[1])>10){
    num_caballos=10;
  }else if(atoi(argv[1])<2){
    num_caballos=2;
  }else{
    num_caballos=atoi(argv[1]);
  }

  long_carrera=atoi(argv[2]);

  if(atoi(argv[3])>100){
    num_apostadores=100;
  }else{
    num_apostadores=atoi(argv[3]);
  }

  num_ventanillas=atoi(argv[4]);

  dinero=atoi(argv[5]);

  remove(PATH);

  active=(int*)malloc(num_caballos*sizeof(int));
  if(!active){
    perror("malloc error");
    return(EXIT_FAILURE);
  }
  for(int i=0;i<num_caballos;i++){
    active[i]=i;
  }

  if(crear_semaforos(num_caballos)==-1){
    printf("Error creando los semaforos\n");
    fflush(stdout);
    free(active);
    return(EXIT_FAILURE);
  }

  if(crear_memoria()==-1){
    printf("Error reservando memoria compartida\n");
    fflush(stdout);
    free(active);
    return(EXIT_FAILURE);
  }


  msgid = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
  if(msgid == -1) {
    free(active);
    perror("Error al obtener identificador para cola de mensajes");
    return -1;
  }

  /* reservamos memoria de las pipes en las que escribe el proceso principal */
  pipes_escritura_principal = (int **)malloc(num_caballos*sizeof(int*));
  if(!pipes_escritura_principal){
    perror("malloc error\n");
    free(active);
    exit(EXIT_FAILURE);
  }
  for(int i=0;i<num_caballos;i++){
    pipes_escritura_principal[i]=(int *)malloc(2*sizeof(int));
    if(!pipes_escritura_principal[i]){
      perror("malloc error\n");
      free(active);
      for(int j=0;j<i;j++){
        free(pipes_escritura_principal[j]);
      }
      free(pipes_escritura_principal);
      exit(EXIT_FAILURE);
    }
  }

  /* reservamos memoria de las pipes de las que lee el proceso principal */
  pipes_lectura_principal = (int **)malloc(num_caballos*sizeof(int*));
  if(!pipes_lectura_principal){
    perror("malloc error\n");
    free(active);
    /* si falla la reserva inicial para las pipes de lectura liberamos las pipes de escritura */
    for(int i=0;i<num_caballos;i++){
      free(pipes_escritura_principal[i]);
    }
    free(pipes_escritura_principal);
    exit(EXIT_FAILURE);
  }
  for(int i=0;i<num_caballos;i++){
    pipes_lectura_principal[i]=(int *)malloc(2*sizeof(int));
    if(!pipes_lectura_principal[i]){
      free(active);
      perror("malloc error\n");
      for(int j=0;j<i;j++){
        free(pipes_lectura_principal[j]);
      }
      free(pipes_lectura_principal);

      /* liberamos tambien las pipes de escritura */
      for(int i=0;i<num_caballos;i++){
        free(pipes_escritura_principal[i]);
      }
      free(pipes_escritura_principal);

      exit(EXIT_FAILURE);
    }
  }

  /*
   * Creacion del proceso monitor, gestor de apuestas y apostador
   * identificador del proceso: 0, 1, 2 respectivamente.
   */

   for(int i=0;i<3;i++){
     identificador=i;
     pid=fork();
     if(pid==-1){
       perror("Error en el fork\n");
       exit(EXIT_FAILURE);
     }else if(pid==0){
       break;
     }else{
       if(i==0){//fork del monitor
         pid_monitor=pid;
       }else if(i==1){
         pid_gestor=pid;
       }else if(i==2){
         pid_apostador=pid;
       }
     }
   }

   /*
    * Creacion de los procesos hijos (caballos) y tuberias correspondientes
    * identificador comienza en i=100;
    */

  if(pid!=0){
    for(k=0;k<num_caballos;k++){
      pipe_status=pipe(pipes_escritura_principal[k]);

      if(pipe_status==-1){
        printf("Error creando la tuberia %d de escritura principal\n", k);
        exit(EXIT_FAILURE);
      }
      pipe_status=pipe(pipes_lectura_principal[k]);

      if(pipe_status==-1){
        printf("Error creando la tuberia %d de lectura principal\n", k);
        exit(EXIT_FAILURE);
      }

      identificador=k+100;
      pid=fork();
      if(pid==-1){
        perror("Error en el fork\n");
        exit(EXIT_FAILURE);
      }else if(pid==0){
        /* los hijos leen de pipes_escritura_principal y escriben en pipes_lectura_principal */
        close(pipes_escritura_principal[k][1]);
        close(pipes_lectura_principal[k][0]);
        break;
      }else{
        pid_caballos[k]=pid;
        /* el padre, tal y como indica el nombre, lee de pipes_lectura_principal y escribe en pipes_escritura_principal */
        close(pipes_escritura_principal[k][0]);
        close(pipes_lectura_principal[k][1]);
      }
    }
  }

  /*
   * ponemos al padre identificador -1 para que no coincida con ningun otro proceso
   */

  if(pid!=0){
    flags = shmat (id_zone_flags, NULL, 0);
      if (flags == NULL) {
      fprintf (stderr, "Error reserve shared memory \n");
      return -1;
    }

    flags->carrera_iniciada=0;
    flags->carrera_terminada=0;

    // printf("Proceso padre esperando\n");
    // fflush(stdout);
    signal(SIGINT, fin_programa);
    signal(SIGALRM,inicio_carrera);
    pause();//en espera a la señal del monitor
    flags->carrera_iniciada=1;
    /*acabamos con proceso apostador*/
    kill(pid_apostador, SIGUSR2);
    /*acabamos con el proceso gestor*/
    kill(pid_gestor, SIGUSR2);
    // printf("Proceso padre iniciando carrera\n");
    // fflush(stdout);
    identificador=-1;
    int *distancia_recorrida;
    int ronda=0;
    distancia_recorrida=(int *)calloc(num_caballos, sizeof(int));
    if(!distancia_recorrida){
      perror("malloc error");
    }
    /* enviamos a cada caballo una señal para que comience la carrera*/
    for(int i=0;i<num_caballos;i++){
      kill(pid_caballos[i], SIGALRM);
    }
    /* mientras ningun caballo termine realizamos rondas de la carrera */
    while(flags->carrera_terminada==0){
      flags->carrera_terminada=ronda_principal(num_caballos, ronda, pipes_lectura_principal, pipes_escritura_principal, distancia_recorrida, long_carrera);
      ronda++;
      sleep(1);
    }
  }

  /*
   * Caballos corren
   */

  if(identificador>=100){
    srand((unsigned) time(NULL)+identificador);
    signal(SIGALRM, inicio_carrera);
    signal(SIGTERM, SIG_DFL);
    pause();
    int ronda=0;
    while(1){
      ronda_caballo(ronda, pipes_lectura_principal[k][1], pipes_escritura_principal[k][0], identificador-100);
      ronda++;
    }
  }
  /*
   * Proceso monitor
   * identificador==0
   * se encarga de imprimir por pantalla la informacion
   */

  if(identificador==0){
    proceso_monitor();
  }



  /*
   * Proceso que gestiona las apuestas
   * identificador==1
   * se encarga de generar tantos hilos como ventanillas haya
   */

  if(identificador==1){
    proceso_gestor_apuestas(num_caballos, num_ventanillas, num_apostadores, msgid);
  }

  /*
   * Proceso que realiza las apuestas
   * identificador==2
   * se encarga de generar apuestas aleatorias para cada caballo
   */
   if(identificador==2){
     proceso_apostador(num_apostadores, num_caballos, dinero, msgid);
   }



  /*
   *  Padre espera y termina
   */

  if(pid!=0){

    //printf("Padre esperando para acabar\n");
    while(wait(&status)>0);
    //printf("Padre acabando y liberando recursos\n");

    shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_carrera, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_flags, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_apuestas, IPC_RMID, (struct shmid_ds *)NULL);


    Borrar_Semaforo(sem_id_mutex1);
    Borrar_Semaforo(sem_id_mutex2);
    Borrar_Semaforo(sem_id_mutex_apuesta);
    Borrar_Semaforo(sem_id_caballos);
    Borrar_Semaforo(sem_id_info_carrera);

    free(active);

    for(int i=0;i<num_caballos;i++){
      free(pipes_escritura_principal[i]);
      free(pipes_lectura_principal[i]);
    }
    free(pipes_escritura_principal);
    free(pipes_lectura_principal);
  }
  //remove(PATH);

  exit(EXIT_SUCCESS);
}

void fin_carrera(int sig){
  if(sig == SIGUSR1){

    for(int i=0;i<num_caballos;i++){
      kill(pid_caballos[i], SIGTERM);
    }
  }
}

void fin_programa(int sig){
  if(sig == SIGINT){

    shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_carrera, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_flags, IPC_RMID, (struct shmid_ds *)NULL);
    shmctl (id_zone_apuestas, IPC_RMID, (struct shmid_ds *)NULL);


    Borrar_Semaforo(sem_id_mutex1);
    Borrar_Semaforo(sem_id_mutex2);
    Borrar_Semaforo(sem_id_mutex_apuesta);
    Borrar_Semaforo(sem_id_caballos);
    Borrar_Semaforo(sem_id_info_carrera);

    remove(PATH);

    kill(-getpid(), SIGTERM);
  }
}

int ronda_principal(int num_caballos, int ronda, int **pipe_lectura, int **pipe_escritura, int *distancia_recorrida, int long_carrera){
  int avance;
  char buffer[80];
  //int primero, ultimo;
  int *posiciones;
  int max_distancia;
  //int *ganadores;
  info_carrera *info;
  info_apuestas *apuestas;

  posiciones=(int *)malloc(num_caballos*sizeof(int));
  if(!posiciones){
    perror("malloc error:");
    return -1;
  }

  info = shmat (id_zone_carrera, NULL, 0);
  if (info == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    free(posiciones);
    return -1;
  }

  apuestas = shmat (id_zone_apuestas, NULL, 0);
  if (apuestas == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    free(posiciones);
    return -1;
  }

  DownMultiple_Semaforo(sem_id_mutex1, num_caballos, SEM_UNDO, active);

  /* Recibimos la tirada de cada caballo hijo y actualizamos el total avanzado por cada caballo */
  for(int i = 0; i < num_caballos; i++){
    read(pipe_lectura[i][0], buffer, sizeof(buffer));
    avance = atoi(buffer);
    distancia_recorrida[i]+=avance;
    Down_Semaforo(sem_id_info_carrera, i, SEM_UNDO);
    info[i].distancia=distancia_recorrida[i];
    info[i].avance=avance;
    Up_Semaforo(sem_id_info_carrera, i, SEM_UNDO);
  }

  for(int i=0;i<num_caballos;i++){
    if(distancia_recorrida[i]>long_carrera){
      flags->carrera_terminada=1;
    }
  }
  if(flags->carrera_terminada==1){
    max_distancia=distancia_recorrida[0];
    for(int i=1;i<num_caballos;i++){
      if(distancia_recorrida[i]>max_distancia){
        max_distancia=distancia_recorrida[i];
      }
    }
    // ganadores=(int *)calloc(num_caballos, sizeof(int));
    for(int i=0;i<num_caballos;i++){
      if(distancia_recorrida[i]==max_distancia){
        apuestas->ganadores[i]=1;
      }
      else{
        apuestas->ganadores[i]=0;
      }
    }

     for(int i = 0; i < num_caballos; i++){
       kill(pid_caballos[i], SIGTERM);
     }

     UpMultiple_Semaforo(sem_id_mutex2, num_caballos, SEM_UNDO, active);

     kill(pid_monitor, SIGUSR2);

     pause();

     //printf("Carrera terminada desde el padre\n");

     free(posiciones);
     return flags->carrera_terminada;
  }


  /* Operamos con las posiciones */

  get_posiciones(distancia_recorrida, posiciones, num_caballos);

  /* Enviamos a cada hijo su posicion */
  for(int i=0;i<num_caballos;i++){
    sprintf(buffer, "%d", posiciones[i]);
    write(pipe_escritura[i][1], buffer, sizeof(buffer));
  }

  sleep(1);
  /* enviamos una señal al monitor para que muestre la situacion actual antes de
   * hacer el up y que los caballos realicen otra tirada.
   */
  kill(pid_monitor, SIGUSR2);

  UpMultiple_Semaforo(sem_id_mutex2, num_caballos, SEM_UNDO, active);

  free(posiciones);

  return flags->carrera_terminada;
}

void ronda_caballo(int ronda, int pipe_escritura, int pipe_lectura, int num_semaforo){
  char buffer[80];
  int posicion;
  int resultado;

  if(ronda == 0){
    resultado = caballo_tira_dado(MEDIO);
  }
  else{
    /* leemos de la tuberia la posicion que nos envia el proceso principal */
    read(pipe_lectura, buffer, sizeof(buffer));
    posicion = atoi(buffer);
    /* generamos una tirada */
    resultado = caballo_tira_dado(posicion);
    sprintf(buffer, "%d", resultado);
  }
  /* mandamos al proceso principal cuanto avanzamos en esta ronda */
  write(pipe_escritura, buffer, sizeof(buffer));

  Up_Semaforo(sem_id_mutex1, num_semaforo,SEM_UNDO);

  Down_Semaforo(sem_id_mutex2, num_semaforo, SEM_UNDO);
}

void get_posiciones(int *distancia_recorrida, int *posiciones, int num_caballos){
  int ultima_posicion;
  int primera_posicion;

  if(!distancia_recorrida || !posiciones){
    perror("get posiciones argumentos incorrectos");
    return;
  }

  primera_posicion=distancia_recorrida[0];
  ultima_posicion=distancia_recorrida[0];

  for(int i=1; i<num_caballos;i++){
    if(distancia_recorrida[i]<ultima_posicion){
      ultima_posicion=distancia_recorrida[i];
    }else if(distancia_recorrida[i]>primera_posicion){
      primera_posicion=distancia_recorrida[i];
    }
  }

  if(primera_posicion==ultima_posicion){ //todos los caballos van en la misma posicion
    return;
  }

  for(int i=0;i<num_caballos;i++){
    if(distancia_recorrida[i]==ultima_posicion){
      posiciones[i]=0;
    }else if(distancia_recorrida[i]==primera_posicion){
      posiciones[i]=2;
    }else{
      posiciones[i]=1;
    }
  }
}

int crear_semaforos(int num_caballos){
  int retorno;
  unsigned short *valor;
  unsigned short mutex=1;

  valor=(unsigned short *)calloc(num_caballos,sizeof(valor));

  retorno=Crear_Semaforo(IPC_PRIVATE,num_caballos,&sem_id_mutex1);
  if(retorno==ERROR || retorno==1){
    printf("Error al crear el semaforo\n");
    fflush(stdout);
    return -1;
  }

  retorno=Crear_Semaforo(IPC_PRIVATE,num_caballos,&sem_id_mutex2);
  if(retorno==ERROR || retorno==1){
    printf("Error al crear el semaforo\n");
    fflush(stdout);
    return -1;
  }

  retorno=Crear_Semaforo(IPC_PRIVATE,num_caballos,&sem_id_caballos);
  if(retorno==ERROR || retorno==1){
    printf("Error al crear el semaforo\n");
    fflush(stdout);
    return -1;
  }

  retorno=Crear_Semaforo(IPC_PRIVATE,num_caballos,&sem_id_info_carrera);
  if(retorno==ERROR || retorno==1){
    printf("Error al crear el semaforo\n");
    fflush(stdout);
    return -1;
  }

  retorno=Crear_Semaforo(IPC_PRIVATE,1,&sem_id_mutex_apuesta);
  if(retorno==ERROR || retorno==1){
    printf("Error al crear el semaforo\n");
    fflush(stdout);
    return -1;
  }

  retorno=Inicializar_Semaforo(sem_id_mutex1,valor,num_caballos);
  if(retorno==ERROR){
    printf("Error al inicializar el semeforo\n");
    return -1;
  }

  retorno=Inicializar_Semaforo(sem_id_mutex2,valor,num_caballos);
  if(retorno==ERROR){
    printf("Error al inicializar el semeforo\n");
    return -1;
  }

  for(int i=0;i<num_caballos;i++){
    valor[i]=1;
  }

  retorno=Inicializar_Semaforo(sem_id_caballos,valor,num_caballos);
  if(retorno==ERROR){
    printf("Error al inicializar el semeforo\n");
    return -1;
  }

  retorno=Inicializar_Semaforo(sem_id_info_carrera,valor,num_caballos);
  if(retorno==ERROR){
    printf("Error al inicializar el semeforo\n");
    return -1;
  }

  retorno=Inicializar_Semaforo(sem_id_mutex_apuesta, &mutex , 1);
  if(retorno==ERROR){
    printf("Error al inicializar el semeforo\n");
    return -1;
  }

  free(valor);

  return 0;
}


void inicio_carrera(int sig){
  if(sig==SIGALRM){
    //printf("Señal recibida para comenzar la carrera\n");
    return;
  }
}
