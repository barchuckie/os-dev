#include <signal.h>
#include <stdio.h>

void hdl(int sig, siginfo_t *siginfo, void *context)
{
	printf ("Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_pid, (long)siginfo->si_uid);
	sleep(1);
	printf("Signal handled\n\n");

}

int main(int argc, char const *argv[]) {
    struct sigaction act;

    act.sa_handler=hdl;
    act.sa_flags=0;

    if(sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    while(1)
        sleep(5);

    printf("FINISH\n");

    return 0;
}
