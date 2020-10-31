#ifndef PIPES_H
#define PIPES_H
#include "argsmap.h"

void duplicator(int pfd, int fd);

void pipe_handler(std::vector<ArgsMap<Symbol, std::vector<std::string>>>& args_map);

#endif // PIPES_H
