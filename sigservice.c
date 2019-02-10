#include <signal.h>
#include <stdio.h>

/*void ctrl_c_handler(int dummy) {
    printf("Control+C\n");
}*/

void hdl(int sig, siginfo_t *siginfo, void *context)
{
	printf ("\nSignal sent: %d\n", sig);
}

int main(int argc, char const *argv[]) {
    //signal(SIGINT, ctrl_c_handler);
    int sig;

    struct sigaction act;

    act.sa_handler=hdl;
    act.sa_flags=0;

    sig = atoi(argv[1]);

    if(sigaction(sig, &act, NULL) < 0) {
        perror("sigaction cannot handle this signal");
        return 1;
    }

    sleep(50);

    printf("FINISH\n");

    return 0;
}
