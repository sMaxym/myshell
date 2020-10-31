#include "../include/server.h"
#include <filesystem>
#include <iostream>

#include <filesystem>
#include <vector>
#include <sstream>
#include "../include/inter_functions.h"
#include <iterator>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/algorithm/string.hpp>
#include "../include/pipes.h"
#include "../include/execution.h"
#include "../include/tcp_server.h"

void client_handler(TcpServer& server, int psd) {
    std::string msg = "Welcome to sHELL\n";
    server.write(psd, msg);
    dup2(psd, 1);
    dup2(psd, 2);
    while (true) {
        std::vector<std::string> line_vector;
        auto cwd = std::filesystem::current_path().string();
        std::stringstream ss;
        if (errno == 0) {
            ss << "\033[1;32m" <<  errno;
        } else {
            ss << "\033[1;31m" <<  errno;
        }

        ss << " " << "\033[0;33m" << cwd;
        ss << "\033[1;33m" << " $ " << "\033[1;39m";

        server.write(psd, ss.str());
        std::string line = server.recieve(psd);

        boost::split( line_vector, line.substr(0, line.size() - line.find('#')), boost::is_any_of("|"));
        std::vector<ArgsMap<Symbol, std::vector<std::string>>> args_maps;
        for (auto& st: line_vector) {
            ArgsMap args_map = parse_line(st);
            args_map.get_tree_map()[CURRENT_PATH].push_back(cwd);
            args_maps.push_back(args_map);

        }

        if (args_maps.size() == 1) {
            if (kernel_command(args_maps[0].get_tree_map()) < 0) {
                handle_exec(args_maps[0]);
            }
        } else {
            pipe_handler(args_maps);
        }

    }
}

void run_server(int port)
{
    try {
        std::cout << "Creating Server!" << std::endl;
        TcpServer server(port);

        std::cout << "Server Created!" << std::endl;
        server.listen_socket();
        std::cout << "Listen Begin!" << std::endl;
        while (true) {

            int psd = server.accept_socket();
            pid_t pid = fork();
            if (pid == -1 ) {
                std::cerr << "Failed to create pid" << std::endl;
            } else if (pid == 0) {
                client_handler(server, psd);
            }
            sleep(2);
        }
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(12);
    }
}
