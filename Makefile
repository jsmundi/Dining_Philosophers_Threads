
CFLAGS = -std=c99 -Wall -pedantic -g  -c -g

philo: random.o philo.o
	gcc -pthread random.o philo.o -o philo -lm
	
philo.o: philo.c random.h
	gcc $(CFLAGS) philo.c

random.o: random.c random.h
	gcc $(CFLAGS) random.c

clean:
	rm -f *.o philo

run: philo
	./philo

all: philo philo.o random.o 
