CC = gcc
CFLAGS = -g -D_POSIX_C_SOURCE -D_GNU_SOURCE -std=c99 -Wall
LDFLAGS = -lm -lpthread

EXE = carrera

all : $(EXE)

clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o caballo.o aleat_num.o semaforos.o monitor.o gestor_apuestas.o apostador.o memoria.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
