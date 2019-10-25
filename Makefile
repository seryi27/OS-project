CC = gcc
CFLAGS = -g -D_POSIX_C_SOURCE -D_GNU_SOURCE -std=c99 -Wall
LDFLAGS = -lm -lpthread

EXE = src/carrera

all : $(EXE)

clean :
	rm -f src/*.o core $(EXE)

$(EXE) : % : %.o src/caballo.o src/aleat_num.o src/semaforos.o src/monitor.o src/gestor_apuestas.o src/apostador.o src/memoria.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
