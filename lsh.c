#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LSH_SIZE 1024
#define TOKEN_DELIMITER " \n\t\r\a"
#define MAX_TOKEN_SIZE 64
#define MAX_CHILDREN_IN_BG 100
#define MAX_COMMAND 100

pid_t children[MAX_CHILDREN_IN_BG];
int child_count;
pid_t curchild;

/********************
 *  Builtin methods *
 ********************/

struct Builtin {
    char *name;
    void (*lsh_func) (char**);
};

void lsh_cd(char **command) {
    if(command[1] == NULL) {
        printf("lsh: no argument for \"cd\"\n");
    } else if(chdir(command[1]) != 0){
        perror("lsh");
    }
}

void lsh_exit(char **command) {
    int i;
    for(i = 0; i < child_count; ++i) {
        kill(children[i], SIGTERM);
    }
    free(command);
    printf("\033[38;5;48m");
    printf("\n\n************************************************\n");
    printf("********        Exiting...   Bye!       ********\n");
    printf("************************************************\n");
    printf("\033[0m");
    exit(0);
}

struct Builtin builtins[] = {
    { "cd", &lsh_cd },
    { "exit", &lsh_exit },
};

int builtins_size() {
    return sizeof(builtins) / sizeof(struct Builtin);
}


/************************************************
 *  Functions which are particular steps of lsh *
 ************************************************/

/*  Reading standard input  */
char* lsh_read() {
    int bufsize = MAX_LSH_SIZE;
    int c;
    int pos = 0;
    char *buf = malloc(bufsize * sizeof(char));

    c = getchar();
    while (c != '\n') {
        if(c == EOF) {
            lsh_exit(NULL);
        }
        buf[pos] = c;
        pos++;
        c = getchar();
    }

    buf[pos] = '\0';
    return buf;
}

/*  Parsing pipes   */
char** parse_pipes(char* read) {
    int bufsize = MAX_LSH_SIZE;
    int pos = 0;
    const char *delimiter = "|";
    char *token;
    char **commands = malloc(bufsize * sizeof(char*));

    token = strtok(read, delimiter);

    while (token != NULL) {
        commands[pos] = token;
        pos++;

        token = strtok(NULL, delimiter);
    }

    commands[pos] = NULL;
    return commands;
}

/*  Parsing to command */
char** to_command(char* read) {
    int bufsize = MAX_TOKEN_SIZE;
    int pos = 0;
    const char *delimiter = TOKEN_DELIMITER;
    char *token;
    char **command = malloc(bufsize * sizeof(char*));

    token = strtok(read, delimiter);

    while (token != NULL) {
        command[pos] = token;
        pos++;

        token = strtok(NULL, delimiter);
    }

    command[pos] = NULL;
    return command;
}

/*  Check if redirect and do so if true */
void potential_redirection(char** command) {
    int pos = 0;
    int fd;

    while(command[pos] != NULL) {
        if(strcmp(command[pos],">") == 0) {
            if(command[pos+1] == NULL) {
                perror("wrong syntax");
            }
            fd = open(command[pos+1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
            if(dup2(fd, 1) != 1) {
                perror("dup2(file descriptor)");
                exit(1);
            }
            close(fd);
            command[pos] = NULL;
        } else if(strcmp(command[pos],"<") == 0) {
            if(command[pos+1] == NULL) {
                perror("wrong syntax");
            }
            fd = open(command[pos+1], O_RDONLY, 0666);
            if(dup2(fd, 0) != 0) {
                perror("dup2(file descriptor)");
                exit(1);
            }
            close(fd);
            command[pos] = NULL;
        } else if(strcmp(command[pos],"2>") == 0) {
            if(command[pos+1] == NULL) {
                perror("wrong syntax");
            }
            fd = open(command[pos+1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
            if(dup2(fd, 2) != 2) {
                perror("dup2(file descriptor)");
                exit(1);
            }
            close(fd);
            command[pos] = NULL;
        }
        pos++;
    }
}


/*  Launching command from PATH */
void launch(char** command) {
    int status;
    int pid;
    int in_background = 0;
    int i = 0;
    int save_in = dup(0);
    int save_out = dup(1);
    int save_err = dup(2);

    /* Checking if execute in background */
    while(command[i] != NULL) {
        i++;
    }
    if(strcmp(command[i-1],"&") == 0) {
        in_background = 1;
        command[i-1] = command[i];
    }

    pid = fork();

    if(pid == 0) {
        /* Child process */
        potential_redirection(command);
        execvp(command[0], command);
        perror("lsh execvp");
        exit(1);
    } else if(pid < 0) {
        /* Error */
        perror("lsh pid < 0");
        exit(1);
    } else {
        /* Parent process */
        if(in_background == 0) {
            curchild = pid;
            waitpid(pid, &status, 0);
        } else {
            children[child_count] = pid;
            child_count++;
        }
        dup2(save_in, 0);
        dup2(save_out, 1);
        dup2(save_err, 2);
    }
}

/*  Executing given command */
void execute(char** command) {
    int i;

    if(command[0] == NULL) {
        /* Nothing entered */
        return;
    }

    /* Checking if builtin method call */
    for(i = 0; i < builtins_size(); ++i) {
        if(strcmp(command[0], builtins[i].name) == 0) {
            builtins[i].lsh_func(command);
            return;
        }
    }

    /* Launching command from $PATH */
    launch(command);
}

/*  Executing pipes */
void execute_pipes(char** commands_in_pipes) {
    int maxsize = MAX_COMMAND;
    char** command = to_command(commands_in_pipes[0]);
    char** last_command;
    int num_of_pipes, i, pid, status;
    int num_of_cmds = 0;
    int pos = 0;
    int in_background = 0;

    /*  Counting number of commands in pipes    */
    while(commands_in_pipes[num_of_cmds] != NULL) {
        num_of_cmds++;
    }

    /*  Setting number of pipes */
    num_of_pipes = num_of_cmds - 1;

    /* Checking if execute in background */
    last_command = to_command(commands_in_pipes[num_of_cmds-1]);
    i = 0;
    while(last_command[i] != NULL) {
        i++;
    }
    if(strcmp(last_command[i-1],"&") == 0) {
        in_background = 1;
        last_command[i-1] = last_command[i];
    }

    /*  Creating all needed pipes   */
    int pipefd[num_of_pipes*2];

    for(i = 0; i < num_of_pipes; ++i) {
        if(pipe(pipefd + i*2) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    int index;
    while(pos < num_of_cmds) {
        pid = fork();
        if(pid == 0) {
            if(pos > 0) {
                index = (pos-1)*2;
                if(dup2(pipefd[index], 0) != 0) {
                    perror("dup2(pipefd)");
                    exit(1);
                }
            }
            if(pos < num_of_cmds-1) {
                index = pos*2+1;
                if(dup2(pipefd[index], 1) != 1) {
                    perror("dup2(pipefd)");
                    exit(1);
                }
            }
            for(i = 0; i < num_of_pipes*2; ++i) {
                close(pipefd[i]);
            }
            potential_redirection(command);
            execvp(command[0], command);
            perror("command in pipe");
            exit(1);
        } else if(pid < 0) {
            perror("fork in pipe");
            exit(1);
        }

        pos++;
        command = to_command(commands_in_pipes[pos]);
        if(pos == num_of_cmds-1) {
            command = last_command;
        }
        curchild = pid;
        children[child_count] = pid;
        child_count++;
    }

    for(i = 0; i < num_of_pipes*2; ++i) {
        close(pipefd[i]);
    }

    if(in_background == 0) {
        for(int j = 0; j < num_of_cmds; ++j) {
            wait(&status);
        }
    }
}

/**********************************************************/

/*  Printing current directory  */
void print_cwd() {
    char cwd[MAX_LSH_SIZE];
    getcwd(cwd, sizeof(cwd));
    int pos = 0;
    const char *delimiter = "/";
    char *token;
    char **folders = malloc(100 * sizeof(char*));

    token = strtok(cwd, delimiter);

    while (token != NULL) {
        folders[pos] = token;
        pos++;

        token = strtok(NULL, delimiter);
    }
    printf("%s", folders[pos-1]);
}


void lsh_loop() {
    char *user_input;
    char **commands_in_pipes;
    char **command;
    while (1) {
        printf("\033[38;5;48m");
        printf("lsh\033[0m:");
        printf("\033[38;5;240m");
        print_cwd();
        printf("\033[1;38;5;124m");
        printf(" barchuckie\033[0;38;5;48m$\033[0m ");
        user_input = lsh_read();
        commands_in_pipes = parse_pipes(user_input);
        if(commands_in_pipes[1] == NULL) {
            command = to_command(commands_in_pipes[0]);
            execute(command);
            free(command);
        } else {
            execute_pipes(commands_in_pipes);
        }

        free(user_input);
        free(commands_in_pipes);
    }
}

void ctrl_c_handler(int sig) {
	kill(curchild, sig);
    printf("\n");
}
void chld_handler(int sig) {
    wait(NULL);
}

/*  Main func performs lsh lifeloop  */
int main(int argc, char const *argv[]) {
    child_count = 0;
    struct sigaction act;

    act.sa_handler=&ctrl_c_handler;
    act.sa_flags=0;

    if(sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction cannot handle this signal");
        return 1;
    }
    signal(SIGCHLD, chld_handler);

    printf("\033[38;5;48m");
    printf("\n************************************************\n");
    printf("********         WELCOME TO LSH         ********\n");
    printf("********     Author: Patryk Barczak     ********\n");
    printf("************************************************\n");
    printf("\033[0m\n");

    lsh_loop();

    return 0;
}
