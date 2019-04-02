
/*
 * JT Mundi
 * CS 360
 * Assignment 8
 * 
 * Dining Philosophers Threaded
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "random.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define PHILO_MAX 5
#define MAX_EAT_TIME 100
#define THINK_MEAN 11
#define THINK_STDV 7
#define EAT_MEAN 9
#define EAT_STDV 3

// Global
pthread_mutex_t chopstick[5];
int philoNum[5] = {0, 1, 2, 3, 4};

int eat(int id)
{
    int currentEat = randomGaussian(EAT_MEAN, EAT_STDV);

    if (currentEat < 0)
    {
        currentEat = 0;
    }

    printf(ANSI_COLOR_YELLOW "Philo %d is eating for %d seconds.\n" ANSI_COLOR_RESET, id, currentEat);

    if (fflush(stdout))
    {
        fprintf(stderr, "fflush Error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    sleep(currentEat);
    return currentEat;
}

int think(int id)
{
    int currentThink = randomGaussian(THINK_MEAN, THINK_STDV);

    if (currentThink < 0)
    {
        currentThink = 0;
    }

    printf(ANSI_COLOR_GREEN "Philo %d is thinking for %d seconds.\n" ANSI_COLOR_RESET, id, currentThink);

    if (fflush(stdout))
    {
        fprintf(stderr, "fflush Error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    sleep(currentThink);
    return currentThink;
}

int getChopstick(int id)
{
    if (pthread_mutex_lock(&chopstick[id]))
    {
        fprintf(stderr, "Pthread Mutex Lock Error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_trylock(&chopstick[(id + 1) % 5]) == EBUSY)
    {
        if (pthread_mutex_unlock(&chopstick[id]))
        {
            fprintf(stderr, "Pthread Mutex Unlock Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void *philoT(void *philoID)
{
    int id = *((int *)philoID);
    int ready = 1;
    int totalEat = 0;
    int totalThink = 0;

    while (totalEat < MAX_EAT_TIME)
    {
        // Try to get a chopstick for eating
        while (ready)
        {
            ready = getChopstick(id);
            sleep(1);
        }

        // Total eat time for Philosopher
        totalEat += eat(id);

        printf("***********************************************\n");
        printf(ANSI_COLOR_RED "Philosopher %d total time spent eating %d\n" ANSI_COLOR_RESET, id, totalEat);
        printf("***********************************************\n");

        if (fflush(stdout))
        {
            fprintf(stderr, "fflush Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (pthread_mutex_unlock(&chopstick[id]))
        {
            fprintf(stderr, "Pthread Mutex Unlock Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (pthread_mutex_unlock(&chopstick[(id + 1) % 5]))
        {
            fprintf(stderr, "Pthread Mutex Unlock Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Total think time for Philosopher
        totalThink += think(id);

        printf("***********************************************\n");
        printf("Philosopher %d total time spent thinking %d\n", id, totalThink);
        printf("***********************************************\n");

        if (fflush(stdout))
        {
            fprintf(stderr, "fflush Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    printf(ANSI_COLOR_CYAN "Philosopher %d is done eating and has left the table.\n" ANSI_COLOR_RESET, id);
    printf(ANSI_COLOR_MAGENTA "Philosopher %d ate for total %d seconds and thought for total %d seconds\n" ANSI_COLOR_RESET, id, totalEat, totalThink);

    return 0;
}

int main()
{
    pthread_t philo[PHILO_MAX];

    // Create a new mutex for each chopstick
    for (int i = 0; i < PHILO_MAX; i++)
    {
        if (pthread_mutex_init(&chopstick[i], NULL))
        {
            fprintf(stderr, "Pthread Mutex Init Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Create a new thread for each Philosophers
    for (int j = 0; j < PHILO_MAX; j++)
    {
        if (pthread_create(&philo[j], NULL, philoT, &philoNum[j]))
        {
            fprintf(stderr, "Pthread Create Error | Error Number %d : %s \n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Join and terminate the threads
    for (int k = 0; k < PHILO_MAX; k++)
    {
        pthread_join(philo[k], NULL);
    }

    printf("All philosphers have finished eating and left the table.\n");

    return 0;
}
