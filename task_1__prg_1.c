#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <wait.h>

#define SHM_KEY 0x2121

volatile sig_atomic_t receiver_ready = 0;

void handler (int signum) {
    receiver_ready = 1;
}

int main (int argc, char* argv[]) {
    int buffer_size, shmid;
    pid_t pid;

    printf("[buffer size]: ");
    scanf("%d", &buffer_size);

    shmid = shmget(SHM_KEY, (buffer_size + 2) * sizeof(int), 0666 | IPC_CREAT);
    int* shared_memory = (int*) shmat(shmid, NULL, 0);

    signal(SIGUSR1, handler);

    pid = fork();
    if (pid == 0) {
        char shmid_string[10];
        sprintf(shmid_string, "%d", shmid);

        execl("./program_2", "./program_2", shmid_string, NULL);
        exit(1);
    }

    while (1) {
        int v_count;
        printf("[values count (0 to quit)]: ");
        scanf("%d", &v_count);

        if (v_count > buffer_size) {
            printf("values count exceeds buffer size...\n\n");
            continue;
        }

        if (v_count == 0) {
            kill(pid, SIGTERM);
            break;
        }

        printf("\n");
        for (int k = 0; k < v_count; k++) {
            printf("[value #%d]: ", k + 1);
            scanf("%d", &shared_memory[k + 1]);
        }
        printf("\n");

        shared_memory[0] = v_count;
        receiver_ready = 0;

        kill(pid, SIGUSR1);

        while (!receiver_ready) 
            pause();

        printf("sum is: %d\n\n", shared_memory[v_count + 1]);
    }

    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
