#ifndef SYNTAX_TREE_PARSER_H
#define SYNTAX_TREE_PARSER_H
#include <map>
#include "argsmap.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "wildcards.h"
#include <iostream>
#include <fcntl.h>
#include <sstream>
typedef std::map<Symbol, std::vector<std::string>> tree_map_t;
typedef Tree<Symbol, std::string> tree_t;
static void to_leaf(tree_t* tree, tree_map_t& trm, Symbol symbol,
             size_t children_index = 0);

static void recursive_file(Tree<Symbol, std::string>* tr,
                    std::map<Symbol, std::vector<std::string>>& m,
                           Symbol symbol);

std::string find_var(const std::string& var);

static void recursive_args(Tree<Symbol, std::string>* tr,
                    std::map<Symbol, std::vector<std::string>>& m);

static void recursive_rdcts(Tree<Symbol, std::string>* tr,
                            std::map<Symbol, std::vector<std::string>>& m);

tree_map_t tree2map(Tree<Symbol, std::string>* tr);


#endif // SYNTAX_TREE_PARSER_H
