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

#include "apostador.h"
#include "mensaje.h"
#include "aleat_num.h"
#include "estructuras.h"


extern int id_zone_apuestas;

void proceso_apostador(int num_apostadores, int num_caballos, int dinero_total, int msgid){
  mensaje1 msg;
  //int restante = dinero_total;
  double cantidad;
  int status;
  int pid;
  info_apuestas *apostadores;

  apostadores = shmat (id_zone_apuestas, NULL, 0);
    if (apostadores == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }
  /* generamos tantos procesos hijo como numero de apostadores recibamos como
   * argumento del programa
   */

  signal(SIGUSR2, manejador_terminar);

  for(int i=0; i<num_apostadores; i++){
    if((pid = fork())== -1){
      perror("fork()");
      return;
    }
    else if(pid == 0){
      srand((unsigned) time(NULL)+i);
      apostadores->restante[i] = dinero_total;

      /* cada apostador genera apuestas aleatorias mientras tenga dinero */
      while(apostadores->restante[i] > 0){
        sprintf(msg.nombre, "Apostador-%d", i+1);
        msg.id = 2;
        msg.apostador = i;
        msg.caballo= aleat_num(0, num_caballos-1);
        cantidad = aleat_num(0, dinero_total*100)/100;

        /* si la cantidad a apostar es mayor que el restante apuesta todo lo que le queda */
        if(cantidad >= apostadores->restante[i]){
          cantidad = apostadores->restante[i];
          apostadores->restante[i] = 0;
        }
        else{
          apostadores->restante[i] -= cantidad;
        }
        /* enviamos a las ventanillas un mensaje con la apuesta para que la procesen */
        msg.apuesta = cantidad;

        // printf("apuesta:%lf\n", cantidad);
        // fflush(stdout);

        if(msgsnd(msgid, (struct msgbuf *)&msg, sizeof(mensaje1)-sizeof(long), 0) == -1){
          perror("proceso_apostador ... error mensaje");
          exit(EXIT_FAILURE);
        }
        sleep(1);
      }
      exit(EXIT_SUCCESS);
    }
  }

  while(wait(&status)>0);
  return;
}

void manejador_terminar(int sig){
  if(sig != SIGUSR2){
    return;
  }
  printf("La carrera ha terminado y los apostadores pueden morirse\n");
  fflush(stdout);
  kill(-getpid() , SIGKILL);
}
