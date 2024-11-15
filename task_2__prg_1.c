#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>

#define SHM_KEY 0x2121
#define SEM_KEY 0x3131

struct shared_data {
    int input_value;
    int result;
    int flag;
};

void sem_wait (int semid) {
    struct sembuf sb = { 0, -1, 0 };
    semop(semid, &sb, 1);
}

void sem_signal (int semid) {
    struct sembuf sb = { 0, 1, 0 };
    semop(semid, &sb, 1);
}

int main () {
    int shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data* shm = (struct shared_data*) shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 0);

    printf("server is running...\n");

    while (1) {
        sem_wait(semid);

        if (shm->input_value == 0)
            break;

        shm->result = shm->input_value * 2;
        sem_signal(semid);
    }

    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
