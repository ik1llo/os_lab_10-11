#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 0x2121

volatile sig_atomic_t sender_ready = 0;

void handler (int signum) {
    sender_ready = 1;
}

int main (int argc, char* argv[]) {
    int shmid;
    if (argc < 2)
        exit(1);

    shmid = atoi(argv[1]);
    int* shared_memory = (int*) shmat(shmid, NULL, 0);

    signal(SIGUSR1, handler);

    while (1) {
        while (!sender_ready) 
            pause();

        if (shared_memory[0] == 0) 
            break;

        int v_count = shared_memory[0];
        int v_sum = 0;
        for (int k = 1; k <= v_count; k++)
            v_sum += shared_memory[k];

        shared_memory[v_count + 1] = v_sum;

        sender_ready = 0;
        kill(getppid(), SIGUSR1);
    }

    shmdt(shared_memory);
    
    return 0;
}
