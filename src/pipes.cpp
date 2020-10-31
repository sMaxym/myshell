#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include "../include/pipes.h"
#include "../include/execution.h"
void duplicator(int pfd, int fd) {

    if (pfd != fd) {
        if (dup2(pfd, fd) == -1)
        {
            perror("Failed to dup2 i - 1");
        }
        if (close(pfd) == -1)
        {
            perror("Failed to close read i - 1");
        }
    }
}

void pipe_handler(std::vector<ArgsMap<Symbol, std::vector<std::string>>>& args_map) {
    int process_numbers =  args_map.size();
    std::vector<int[2]> pfds(process_numbers - 1);
    std::vector<pid_t> pids(process_numbers);
    for (int i =0 ; i < process_numbers; ++i) {
        if (i != process_numbers - 1)
        {
            if (pipe(pfds[i]) == -1){
                perror("Faied to pipe");
            }
        }
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Faied to fork");
        }

        else if (pids[i] == 0) {

            if (i != 0) {
                duplicator(pfds[i - 1][0], STDIN_FILENO);
            }
            if (i != process_numbers - 1)
            {

                if (close(pfds[i][0]) == -1){
                    perror("Failed to close read i");
                }

                duplicator(pfds[i][1], STDOUT_FILENO);

            }
            execution(args_map[i]);
        }
        else {
            if (i != 0) {
                if (close(pfds[i - 1][0]) < 0) {
                    perror("failed");
                }
            }
            if (i != process_numbers - 1) {
                if (close(pfds[i][1])) {
                    perror("failed");
                }
            }
        }
    }

    for (int i = 0; i < process_numbers; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
        errno = status;
    }
}
