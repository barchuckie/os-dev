#include <signal.h>
#include <sys/types.h>
#include <stdio.h>


int main(int argc, char const *argv[]) {
    if(argc < 2) {
        printf("No PID given\n");
    }
    int i;
    int pid = atoi(argv[1]);

    union sigval value;

    printf("My PID: %d\n", getpid());
    printf("PID to kill: %d\n", pid);

    for(i = 10; i < 20; ++i) {
        value.sival_int = i;
        int x = sigqueue(pid, SIGUSR1, value);
        //sleep(1);
    }
    return 0;
}
