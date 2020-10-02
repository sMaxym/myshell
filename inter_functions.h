#ifndef INTER_FUNCTIONS_H
#define INTER_FUNCTIONS_H
#include <vector>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "syntax_tree_parser.h"
int kernel_command(tree_map_t& tree_map);
enum command_t {
    mecho_t,
    mexport_t,
    mexit_t,
    mpwd_t,
    mcd_t,
    merrno_t,
    run_script_t

};

static std::unordered_map<std::string, command_t> commands_map = {
    {"mecho", mecho_t},
    {"merrno", merrno_t},
    {"mexit", mexit_t},
    {"mexport", mexport_t},
    {"mpwd", mpwd_t},
    {"mcd", mcd_t},
    {"merrno", merrno_t},
    {".", run_script_t}
};

int flag_founder(const std::vector<std::string>& flags,
                  const std::string& text,
                  const std::string& prg_name) {
    bool help_found = false;
    for (const auto& flag: flags) {
        if (flag != "--help" && flag != "-h") {
            std::cout << prg_name << ": no such option - " << flag << std::endl;
            return 2;
        }
        else {
            help_found = true;
        }
    }
    if (help_found) {
        std::cout << text << std::endl;

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
        int status = setenv(tree_map[KEY][i].c_str(), tree_map[VALUE][i].c_str(), 1);
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
        std::stringstream ss(tree_map[ARGS][0]);
        ss >> x;
        exit(x);
    }
    return 0;

}

void parse_line(std::string& line, const std::string& cwd) {

    init_clean(line);
    auto syntax = ll1_parser(line.c_str());
    ArgsMap args_map(tree2map(syntax->children[0]));

    args_map.get_tree_map()[CURRENT_PATH].push_back(cwd);
//            for (auto& x: args_map.get_tree_map()) {
//                std::cout << x.first << " " << x.second << std::endl;
//            }
    if (kernel_command(args_map.get_tree_map()) < 0) {
        handle_exec(args_map);
    }
}

int run_script(const std::string& script) {
    std::ifstream ifile(script);
    std::string line;
    while (getline(ifile, line, '\n')) {
        if (line.empty()) {
            continue;
        }
        auto cwd = std::filesystem::current_path().string();
        parse_line(line, cwd);
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
    if (commands_map.find(tree_map[PROG][0]) == commands_map.end()) {

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
#endif // INTER_FUNCTIONS_H
