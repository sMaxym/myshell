#include "../include/execution.h"
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <sys/wait.h>

static void dublicate_file(int fd_to, int fd_from) {
    if (dup2(fd_to, fd_from) < 0) {
        close(fd_to);
        std::cerr << "Failed to redirect" << std::endl;
        exit(1);
    }
}

static void handle_redirect(int fd, ArgsMap<Symbol, std::vector<std::string>>& arg_map) {


    if (arg_map.get_tree_map()[T_P_FROM][0].empty()) {
        arg_map.get_tree_map()[T_P_FROM][0] = arg_map.get_tree_map()[T_RDCT][0] == ">" ? "1" : "0";
    }
    else if (arg_map.get_tree_map()[T_P_FROM][0] == "&") {
        dublicate_file(fd, 1);
        dublicate_file(fd, 2);
    }

    int fd_from;
    std::stringstream(arg_map.get_tree_map()[T_P_FROM][0]) >> fd_from;
    dublicate_file(fd,fd_from);
    if (arg_map.get_tree_map()[T_P_FROM].size() == 2) {
        std::stringstream(arg_map.get_tree_map()[T_P_TO][1].substr(1)) >> fd_from;

        dublicate_file(fd, fd_from);
    }
}

void execution(ArgsMap<Symbol, std::vector<std::string>>& arg_map) {
    auto [arg_for_c, prg_name] = arg_map.map2vector();
    if (!arg_map.get_tree_map()[T_P_TO].empty()) {
        int fd = open(arg_map.get_tree_map()[T_P_TO][0].c_str(),
                O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if (fd < 0) {
            std::cerr << "Failed to open " << arg_map.get_tree_map()[T_P_TO][0] << std::endl;
            exit(EXIT_FAILURE);
        }
        handle_redirect(fd, arg_map);
        close(fd);
    }
    execvp(prg_name.c_str(), const_cast<char* const*>(arg_for_c.data()));
    exit(EXIT_FAILURE);
}

void handle_exec(ArgsMap<Symbol, std::vector<std::string>>& arg_map) {
    bool is_back = arg_map.get_tree_map()[T_BG_PRC][0] == "&";
    if (is_back) {
        signal(SIGCHLD,SIG_IGN);
//        close(1);
//        close(2);
//        close(0);
    }
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        if (is_back) {
            close(1);
            close(2);
            close(0);
        }

        execution(arg_map);
    } else {
        if (!is_back) {
            waitpid(pid, &errno, 0);
        }
    }
}
