#include <signal.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  if(argc < 2) {
    printf("No PID given\n");
  }
  int pid = atoi(argv[1]);

  for(int i = 0; i < 20; ++i) {
    kill(pid, 30);
    printf("%d. signal sent\n", i);
  }
  return 0;
}
