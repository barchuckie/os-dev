#include <socketlib.h>

int main(int argc, char const *argv[]) {
    char nickname[NICK_BUF_SIZE];
    int port, sock, fd;

    if(argc != 2) {
        fprintf(stderr, "usage: messenger nickname\n");
        exit(1);
    }

    nickname = (char *)argv[1];

    fd = request_tcp_connection(hn, port);

    write(fd, nickname, NICK_BUF_SIZE);

    printf("Connection made.\n");

    


    execlp("cat", "cat", NULL);
    fprintf(stderr, "Exec failed.\n");

    return 0;
}
