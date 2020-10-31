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
#include <boost/algorithm/string.hpp>
#include "execution.h"
#include "pipes.h"
#include "line_parser.h"
#include "readbuffer.h"

static constexpr int buf_size = 256;


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
                  const std::string& prg_name);
int mecho(tree_map_t& tree_map);

int mexport(tree_map_t& tree_map);

int mexit(tree_map_t& tree_map);

int mpwd(tree_map_t& tree_map);

int mcd(tree_map_t& tree_map);

int merrno(tree_map_t& tree_map);

int run_script(const std::string& script);

int kernel_command(tree_map_t& tree_map);
#endif // INTER_FUNCTIONS_H
