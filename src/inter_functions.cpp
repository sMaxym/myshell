#include "../include/inter_functions.h"

int flag_founder(const std::vector<std::string>& flags,
                  const std::string& text,
                  const std::string& prg_name) {
    bool help_found = false;
    for (const auto& flag: flags) {
        if (flag != "--help" && flag != "-h") {
            std::cerr << prg_name << ": no such option - " << flag << std::endl;
            return 2;
        }
        else {
            help_found = true;
        }
    }
    if (help_found) {
        std::cerr << text << std::endl;

        return 0;
    }
    return 1;
}
int mecho(tree_map_t& tree_map)
{
    std::string text = "mecho [-h|--help] [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>] ...";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    for (const auto& x: tree_map[ARGS]) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}

int mexport(tree_map_t& tree_map)
{
    std::string text = "mexport [-h|--help] [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>] ...";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    if (tree_map[KEY].empty()) {
        return 0;
    }
    for (size_t i = 0; i < tree_map[KEY].size(); ++i) {
        if (tree_map[VALUE][i][0] == 'A') {
            std::string line = tree_map[VALUE][i].substr(1);
            int p[2];
            if(pipe(p) < 0) {
                std::cerr << "Failed to pipe"<< std::endl;
                return 1;
            }
            auto args_maps = line2argsmap(line, tree_map[CURRENT_PATH][0]);

            pid_t pid = fork();
            if (pid == -1) {
                std::cerr << "Failed to fork"<< std::endl;
                return 2;
            }
            else if (pid == 0) {
                if(args_maps.size() == 1) {
                    if (close(p[0]) == -1){
                        perror("Failed to close read i");
                    }
                    duplicator(p[1], STDOUT_FILENO);
                    execution(args_maps[0]);
                } else {

                }


            } else {

                if (close(p[1]) < 0) {
                    perror("Failed to close write at main proc");
                }
                char buf[buf_size];
                waitpid(pid, &errno, 0);
                ssize_t total_read;
                std::string varbuf;
                while(true) {
                     if (readbuffer(p[0], buf_size, buf, total_read) < 0) {
                         close(p[0]);
                         std::cerr << "Failed to read buf" << std::endl;
                         return 3;
                     }
                     if (total_read == 0) {
                         break;
                     } else {
                        varbuf.append(buf, buf + total_read);
                     }
                }

                close(p[0]);
                varbuf.pop_back();
                tree_map[VALUE][i] = varbuf;
            }

        }
        status = setenv(tree_map[KEY][i].c_str(), tree_map[VALUE][i].c_str(), 1);
        if (status != 0) {
            return status;
        }
    }
    return 0;
}

int mexit(tree_map_t& tree_map) {
    std::string text = "mexit [код завершення] [-h|--help]  – вийти із myshell";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    if (tree_map[ARGS].empty()) {
        exit(0);
    } else {
        int x;
        std::stringstream(tree_map[ARGS][0]) >> x;
        exit(x);
    }
    return 0;

}

int run_script(const std::string& script) {
    std::ifstream ifile(script);
    std::string line;
    while (getline(ifile, line, '\n')) {
        if (line.empty()) {
            continue;
        }
        auto cwd = std::filesystem::current_path().string();
        try {
            auto args_maps = line2argsmap(line, cwd);

            if (args_maps.size() == 1) {
                if (kernel_command(args_maps[0].get_tree_map()) < 0) {
                    handle_exec(args_maps[0]);
                }
            } else {
                pipe_handler(args_maps);
            }
        }
        catch (std::runtime_error & e) {
            continue;
        }

    }
    return 0;
}

int mpwd(tree_map_t& tree_map) {
    std::string text = "mpwd [-h|--help] – вивести поточний шлях";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    std::cout << tree_map[CURRENT_PATH][0] << std::endl;
    return 0;
}

int mcd(tree_map_t& tree_map) {
    std::string text = "mcd <path> [-h|--help]  -- перейти до шляху <path>";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    if (tree_map[ARGS].empty()) {
        return 0;
    }
    return chdir(tree_map[ARGS][0].c_str());
}

int merrno(tree_map_t& tree_map) {
    std::string text = "merrno [-h|--help]  – вивести код завершення останньої програми чи команди";
    int status = flag_founder(tree_map[NT_FLAG], text, "mecho");
    if (status != 1)
        return status;
    std::cout << errno << std::endl;
    return 0;
}


int kernel_command(tree_map_t& tree_map) {
    if ((tree_map[PROG].size() > 1) ||
            (commands_map.find(tree_map[PROG][0]) == commands_map.end())) {
        return -1;
    }
    switch(commands_map[tree_map[PROG][0]]) {
        case mecho_t:
            errno = mecho(tree_map);
            break;
        case mexport_t:
            errno = mexport(tree_map);
            break;
        case mexit_t:
            errno = mexit(tree_map);
            break;
        case mpwd_t:
            errno = mpwd(tree_map);
            break;
        case mcd_t:
            errno = mcd(tree_map);
            break;
        case merrno_t:
            errno = merrno(tree_map);
            break;
        case run_script_t:
            errno = run_script(tree_map[ARGS][0]);
            break;
        default:
            break;
    }
    return 0;
}
