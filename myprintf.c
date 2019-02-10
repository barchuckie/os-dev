#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_ITOA_BUFFER 16
#define MAX_SCAN_BUFFER 16

static int itoa_print(int val, int base) {
    int length = 0;
    char buffer[MAX_ITOA_BUFFER];
    char digit;

    do {
        digit = (char)(val % base);
        if (digit < 10) {
            buffer[length++] = '0' + digit;
        } else {
            buffer[length++] = 'A' + digit - 10;
        }
        val = val / base;
    } while(val && length < MAX_ITOA_BUFFER);

    char printable[length];
    for (int i = 0; i < length; i++) {
        printable[i] = buffer[length-1-i];
    }

    if (write(1, printable, length) != length) {
        perror("write");
    }
    return length;
}

int myprintf(const char *format, ...) {
    char *p = (char *) &format + sizeof(*format);
    char *str_arg;
    int int_arg, base;
    int idx = 0;
    //va_list va;
    //va_start(va, format);

    while(*format) {
        if(*format == '%') {
            format++;
        } else {
            if (write(1, format, 1) != 1) {
                perror("write");
            }
            format++;
            idx++;
            continue;
        }

        switch(*format) {
            case 's':
                //str_arg = va_arg(va, char *);
                str_arg = (char *) p;
                p += sizeof(char*);
                while (*str_arg) {
                    if (write(1, str_arg, 1) < 0) {
                        perror("write");
                    }
                    str_arg++;
                    idx++;
                }
                idx--;
                format++;
                break;

            case 'd':
                //int_arg = va_arg(va, int);
                int_arg = *((int *)p);
                p += sizeof(int);
                base = 10;
                idx += itoa_print(int_arg, base);
                format++;
                break;

            case 'x':
                //int_arg = va_arg(va, int);
                int_arg = *((int *)p);
                p += sizeof(int);
                base = 16;
                idx += itoa_print(int_arg, base);
                format++;
                break;

            case 'b':
                //int_arg = va_arg(va, int);
                int_arg = *((int *)p);
                p += sizeof(int);
                base = 2;
                idx += itoa_print(int_arg, base);
                format++;
                break;

            default:
                format--;
                if (write(1, format, 1) != 1) {
                    perror("write");
                }
                format++;
                idx++;
                break;
        }
    }

    //va_end(va);
    p = NULL;
    return idx;
}

static int atoi_scan(char* str) {
    int number = 0;
    while(((*str) >= '0') && ((*str) <= '9')) {
        number = number * 10 + ((*str) - '0');
        str++;
    }
    return number;
}

int myscanf(const char *format, ...) {
    char *p = (char *) &format + sizeof(*format);
    int* int_arg;
    int tmp_int;
    int idx = 0;
    char* tmp;
    char* str_arg;
    //va_list va;
    //va_start(va, format);

    while(*format) {
        if(*format == '%') {
            format++;
        } else {
            format++;
            continue;
        }

        switch(*format) {
            case 's':
                str_arg = p;
                p += sizeof(str_arg);
                //str_arg = va_arg(va, char*);
                if(read(0, tmp, MAX_SCAN_BUFFER) < 0) {
                    perror("read");
                }
                *int_arg = atoi_scan(tmp);
                format++;
                break;
                break;

            case 'd':
                int_arg = *((int *)p);
                p += sizeof(int);
                //int_arg = va_arg(va, int*);
                if(read(0, tmp, MAX_SCAN_BUFFER) < 0) {
                    perror("read");
                }

                tmp_int = 0;
                while(((*tmp) >= '0') && ((*tmp) <= '9')) {
                    tmp_int = tmp_int * 10 + ((*tmp) - '0');
                    tmp++;
                }

                *int_arg = tmp_int;
                format++;
                break;

            case 'x':
                int_arg = *((int *)p);
                p += sizeof(int);
                //int_arg = va_arg(va, int*);
                if(read(0, tmp, MAX_SCAN_BUFFER) < 0) {
                    perror("read");
                }

                tmp_int = 0x0;
                while((((*tmp) >= '0') && ((*tmp) <= '9'))
                    || (((*tmp) >= 'A') && ((*tmp) <= 'A' + 6))) {
                    if(((*tmp) >= '0') && ((*tmp) <= '9')) {
                        tmp_int = tmp_int * 10 + ((*tmp) - '0');
                    } else {
                        tmp_int = tmp_int * 10 + (*tmp);
                    }
                    tmp++;
                }

                *int_arg = tmp_int;
                format++;
                break;

            case 'b':
                int_arg = *((int *)p);
                p += sizeof(int);
                //int_arg = va_arg(va, int*);
                if(read(0, tmp, MAX_SCAN_BUFFER) < 0) {
                    perror("read");
                }

                tmp_int = 0;
                while(((*tmp) == '0') || ((*tmp) == '1')) {
                    tmp_int = tmp_int * 10 + ((*tmp) - '0');
                    tmp++;
                }

                *int_arg = tmp_int;
                format++;
                break;

            default:
                format--;
                if (write(1, format, 1) != 1) {
                    perror("write");
                }
                format++;
                idx++;
                break;
        }
    }

    //va_end(va);
    p = NULL;
    return idx;
}

int main(int argc, char const *argv[]) {
    char* a;
    int i;
    myscanf("%d", &i);
    int idx = myprintf("p ma %x lat\n", i);
    myprintf("Tekst ma dł.: %d\n", idx);
    return 0;
}
