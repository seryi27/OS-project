/**
* @brief Este archivo semaforos.c implementa las funciones declaradas en
*        semáforos.h, las cuales facilitan el uso de semáforos, encapsulando
*        las estructuras y funciones complejas que suponen el uso de semáforos
*        y permitiendo un uso de semáforos más legible y simple.
* @file semaforos.c
* @author Miguel Baquedano miguel.baquedano@estudiante.uam.es
* @author Sergio Cordero sergio.cordero@estudiante.uam.es
* @version 1.0
* @date 02-04-2018
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>

#define ERROR -1
#define OK 0

int Inicializar_Semaforo(int semid, unsigned short *array, int size){
  union semun {
    int val;
    struct semid_ds *semstat;
    unsigned short *array;
  } arg;

  //arg.array = array;
  arg.array=(unsigned short *)malloc(size*sizeof(unsigned short));
  for(int i=0;i<size;i++){
    arg.array[i]=array[i];
  }

  if(semctl (semid, 0, SETALL, arg) == -1){
    perror("Inicializar_Semaforo: semctl");
    free(arg.array);
    return ERROR;
  }
  free(arg.array);
  return OK;
}

int Borrar_Semaforo(int semid){
  if(semctl (semid, 0, IPC_RMID) == -1){
    perror("Borrar_Semaforo: semctl");
    return ERROR;
  }
  return OK;
}

int Crear_Semaforo(key_t key, int size, int *semid){
  unsigned short *valores = NULL;
  int retorno;

  if(!semid){
    printf("Crear_Semaforo: Argumentos invalidos\n");
    fflush(stdout);
    return ERROR;
  }

  valores = calloc(size,sizeof(unsigned short));
  if(!valores){
    perror("Error en el malloc");
    return ERROR;
  }

  *semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if((*semid == -1) && (errno == EEXIST)){
    *semid=semget(key,size,SHM_R|SHM_W);
  }
  if(*semid==-1){
    perror("semget");
    free(valores);
    exit(errno);
  }


  retorno = Inicializar_Semaforo(*semid, valores, size);
  free(valores);
  if(retorno == -1){
    printf("Crear_SemaforoError retorno==ERROR\n");
    fflush(stdout);
    return ERROR;
  }
  return OK;
}

int Down_Semaforo(int id, int num_sem, int undo){
  struct sembuf sem_oper;

  sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
  sem_oper.sem_op =-1;  /* Decrementar en 1 el valor del semáforo */
  sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
                              proceso acaba inesperadamente */
  if(semop(id, &sem_oper, 1) == -1){
    perror("Error al hacer semop(DOWN)");
    return ERROR;
  }
  return OK;
}

int DownMultiple_Semaforo(int id,int size,int undo,int *active){
  struct sembuf *sem_oper;

  sem_oper = calloc(size, sizeof(struct sembuf));
  if(!sem_oper){
    return -1;
  }

  for(int i=0; i<size; i++){
    sem_oper[i].sem_num = active[i]; /* Actuamos sobre el semáforo 0 de la lista */
    sem_oper[i].sem_op =-1;  /* Decrementar en 1 el valor del semáforo */
    sem_oper[i].sem_flg = undo; /* Para evitar interbloqueos si un
                                proceso acaba inesperadamente */
  }

  if(semop(id, sem_oper, size) == -1){
    free(sem_oper);
    return -1;
  }
  free(sem_oper);
  return 0;
}

int Up_Semaforo(int id, int num_sem, int undo){
  struct sembuf sem_oper;

  sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo 0 de la lista */
  sem_oper.sem_op =1;  /* Aumentar en 1 el valor del semáforo */
  sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
                              proceso acaba inesperadamente */
  if(semop(id, &sem_oper, 1) == -1){
    perror("Error al hacer semop(UP)");
    return -1;
  }
  return 0;
}

int UpMultiple_Semaforo(int id,int size, int undo, int *active){
  struct sembuf *sem_oper;

  sem_oper = calloc(size, sizeof(struct sembuf));
  if(!sem_oper){
    return -1;
  }

  for(int i=0; i<size; i++){
    sem_oper[i].sem_num = active[i]; /* Actuamos sobre el semáforo 0 de la lista */
    sem_oper[i].sem_op =1;  /* Aumentar en 1 el valor del semáforo */
    sem_oper[i].sem_flg = undo; /* Para evitar interbloqueos si un
                                proceso acaba inesperadamente */
  }

  if(semop(id, sem_oper, size) == -1){
    free(sem_oper);
    return -1;
  }
  free(sem_oper);
  return 0;
}
