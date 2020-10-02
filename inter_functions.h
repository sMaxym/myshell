#ifndef INTER_FUNCTIONS_H
#define INTER_FUNCTIONS_H
#include <vector>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "argsmap.h"

typedef std::map<Symbol, std::vector<std::string>> tree_map_t;
enum command_t {
    mecho_t,
    mexport_t,
    mexit_t,
    mpwd_t,
    mcd_t,
    merrno_t,

};

static std::unordered_map<std::string, command_t> commands_map = {
    {"mecho", mecho_t},
    {"merrno", merrno_t},
    {"mexit", mexit_t},
    {"mexport", mexport_t},
    {"mpwd", mpwd_t},
    {"mcd", mcd_t},
    {"merrno", merrno_t}
};
void mecho(tree_map_t& tree_map)
{
    for (const auto& x: tree_map[ARGS]) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

void mexport(tree_map_t& tree_map)
{
    setenv(tree_map[KEY][0].c_str(), tree_map[VALUE][0].c_str(), 1);
}

void mexit(tree_map_t& tree_map) {
    std::stringstream ss(tree_map[ARGS][0]);
    int x;
    ss >> x;
    exit(x);
}

void mpwd(tree_map_t& tree_map) {
    std::cout << tree_map[CURRENT_PATH][0] << std::endl;
}

void mcd(tree_map_t& tree_map) {
    errno = chdir(tree_map[ARGS][0].c_str());
}

void merrno() {
    std::cout << errno << std::endl;
}


int kernel_command(tree_map_t& tree_map) {
    if (commands_map.find(tree_map[PROG][0]) == commands_map.end()) {

        return -1;
    }
    switch(commands_map[tree_map[PROG][0]]) {
        case mecho_t:
            mecho(tree_map);
            break;
        case mexport_t:
            mexport(tree_map);
            break;
        case mexit_t:
            mexit(tree_map);
            break;
        case mpwd_t:
            mpwd(tree_map);
            break;
        case mcd_t:
            mcd(tree_map);
            break;
        case merrno_t:
            merrno();
            break;
        default:
            break;
    }
    return 0;
}
#endif // INTER_FUNCTIONS_H
