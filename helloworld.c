/*
    Printing "Hello world!" in different terminal colors
    using ANSII escape sequences.
    Author: Patryk Barczak
*/

#include <stdio.h>

int main(void) {
    for(int i = 0; i < 256; ++i) {
        printf("\033[38;5;%dm%d Hello world!\n", i, i);
    }
    printf("\033[0m");
    return 0;
}
