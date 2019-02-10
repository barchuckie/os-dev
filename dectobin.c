#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef MAX_NUMBER_SIZE
#define MAX_NUMBER_SIZE 1000
#endif

int main(int argc, char const *argv[]) {
    if(argc < 3) {
        fprintf(stderr, "Usage: dtb <number> <number of digits after dot>\n");
        exit(1);
    }

    char* number = (char *) argv[1];
    int digit_count = atoi(argv[2]);
    int idx = 1;
    int pos = 1;
    int digits [MAX_NUMBER_SIZE];
    int int_part = 0;
    int result = 0;

    digits[0] = 0;

    while(*number != '.') {
        if(!*number) {
            number--;
            break;
        }
        int digit = *number - '0';
        int_part += pos*(digit);
        pos *= 10;
        number++;
    }
    number++;

    while(*number) {
        digits[idx] = *number - '0';
        idx++;
        number++;
    }

    pos = 1;
    while(int_part) {
        result += pos*(int_part % 2);
        pos *= 10;
        int_part /= 2;
    }

    printf("Result: %d.", result);

    for(int i = 0; i < digit_count; ++i) {
        for(int j = idx-1; j >= 0; --j) {
            digits[j] *= 2;
            if(j < idx-1 && digits[j+1] > 9) {
                digits[j+1] -= 10;
                digits[j] += 1;
            }
        }

        printf("%d", digits[0]);
        digits[0] = 0;
    }

    printf("\n");

    return 0;
}
