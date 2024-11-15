#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHM_KEY 0x2121
#define SEM_KEY 0x3131
#define DEFAULT_VALUES_SIZE 10

struct SharedData {
    int values[DEFAULT_VALUES_SIZE];
    int v_count;
    int v_sum;
};

void wait_sem (int semid) {
    struct sembuf p = { 0, -1, 0 };
    semop(semid, &p, 1);
}

void signal_sem (int semid) {
    struct sembuf v = { 0, 1, 0 };
    semop(semid, &v, 1);
}

int main () {
    int shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666 | IPC_CREAT);
    struct SharedData* data = (struct SharedData*) shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 0);

    printf("[values count]: ");
    scanf("%d", &data->v_count);
    printf("\n");

    for (int k = 0; k < data->v_count; k++) {
        printf("[value #%d]: ", k + 1);
        scanf("%d", &data->values[k]);
    }

    signal_sem(semid);
    
    wait_sem(semid);

    printf("\n");
    printf("sum is: %d\n", data->v_sum);

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
