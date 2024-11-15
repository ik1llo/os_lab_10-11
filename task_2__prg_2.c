#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

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
    int shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0666);
    struct shared_data* shm = (struct shared_data*) shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 1, 0666);

    printf("client is running...\n\n");

    while (1) {
        int value;
        printf("[value (0 to exit)]: ");
        scanf("%d", &value);

        if (value == 0) {
            shm->input_value = 0;
            sem_signal(semid);

            break;
        }


        shm->input_value = value;

        sem_signal(semid);

        sem_wait(semid);
        printf("multiplied value: %d\n\n", shm->result);
    }

    shmdt(shm);

    return 0;
}
