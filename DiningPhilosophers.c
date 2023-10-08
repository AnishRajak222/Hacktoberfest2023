#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILOSOPHERS 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t mutex;
sem_t forks[NUM_PHILOSOPHERS];

int state[NUM_PHILOSOPHERS];

void think(int philosopher_id) {
    printf("Philosopher %d is thinking\n", philosopher_id);
}

void eat(int philosopher_id) {
    printf("Philosopher %d is eating\n", philosopher_id);
}

void take_forks(int philosopher_id) {
    sem_wait(&mutex);
    state[philosopher_id] = HUNGRY;
    printf("Philosopher %d is hungry\n", philosopher_id);
    test(philosopher_id);
    sem_post(&mutex);
    sem_wait(&forks[philosopher_id]);
}

void put_forks(int philosopher_id) {
    sem_wait(&mutex);
    state[philosopher_id] = THINKING;
    printf("Philosopher %d is putting forks down\n", philosopher_id);
    test((philosopher_id + 4) % NUM_PHILOSOPHERS);
    test((philosopher_id + 1) % NUM_PHILOSOPHERS);
    sem_post(&mutex);
}

void test(int philosopher_id) {
    if (state[philosopher_id] == HUNGRY &&
        state[(philosopher_id + 4) % NUM_PHILOSOPHERS] != EATING &&
        state[(philosopher_id + 1) % NUM_PHILOSOPHERS] != EATING) {
        state[philosopher_id] = EATING;
        printf("Philosopher %d takes forks and starts eating\n", philosopher_id);
        sem_post(&forks[philosopher_id]);
    }
}

void* philosopher(void* philosopher_id) {
    int id = *((int*)philosopher_id);

    while (1) {
        think(id);
        take_forks(id);
        eat(id);
        put_forks(id);
    }
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int philosopher_ids[NUM_PHILOSOPHERS];

    sem_init(&mutex, 0, 1);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&forks[i], 0, 0);
        philosopher_ids[i] = i;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
