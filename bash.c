#include <stdio.h>
#include <unistd.h>

/*
    Po kompilacji (program nazwany "bash"):
    - zmiana właściciela na root: $ sudo chown root ./bash
    - nadanie uprawnień użytkownikowi do wykonania: $ sudo chmod 4755 ./bash
*/

int main(int argc, char const *argv[]) {
    int i;

    setuid(0);

    i = execl("/bin/bash", "-l" ,NULL);
    perror("bash: błąd");
    return 1;
}
