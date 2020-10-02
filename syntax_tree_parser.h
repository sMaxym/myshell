#ifndef SYNTAX_TREE_PARSER_H
#define SYNTAX_TREE_PARSER_H
#include <map>
#include "argsmap.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
typedef std::map<Symbol, std::vector<std::string>> tree_map_t;
typedef Tree<Symbol, std::string> tree_t;
static void to_leaf(tree_t* tree, tree_map_t& trm, Symbol symbol,
             size_t children_index = 0) {
    auto cursor = tree;
    while (!cursor->is_leaf()) {
        cursor = cursor->children[children_index];
    }
    trm[symbol].push_back(cursor->terminal);
}
static void recursive_file(Tree<Symbol, std::string>* tr,
                    std::map<Symbol, std::vector<std::string>>& m,
                           Symbol symbol) {
    if (tr->children.size() > 2) {
        recursive_file(tr->children[0], m, symbol);
        recursive_file(tr->children[2], m, symbol);
    }
    else {
        to_leaf(tr, m, symbol);
    }

}

std::string find_var(const std::string& var) {
    auto val = getenv(var.c_str());
    if (val != nullptr) {
        return std::string(val);
    }
    return std::string();
}

static void recursive_args(Tree<Symbol, std::string>* tr,
                    std::map<Symbol, std::vector<std::string>>& m) {
    if (tr->is_leaf()) {
        return;
    }
    if (tr->non_terminal == NT_ARG) {
        if (tr->children[0]->non_terminal == NT_KEY) {
            to_leaf(tr->children[0], m, KEY);
            if (tr->children[2]->children[0]->non_terminal == NT_VARCALL)
            {

                auto new_arg = find_var(
                            tr->children[2]->children[0]->children[1]->children[0]->terminal
                        );
                m[VALUE].push_back(new_arg);
            }
            else
            {
                to_leaf(tr->children[2], m, VALUE);
            }

        }
        else if (tr->children[0]->non_terminal == NT_VAL)  {
            if (tr->children[0]->children[0]->non_terminal == NT_VARCALL) {
                auto new_arg = find_var(
                            tr->children[0]->children[0]->
                        children[1]->children[0]->terminal
                        );
                m[ARGS].push_back(new_arg);
            }
            else {
                recursive_file(tr->children[0]->children[0], m, FILE_ARGS);
                std::string arg;
                for (auto& x: m[FILE_ARGS]) {
                    if (x == "~") {
                        auto home_dir = getenv("HOME");
                        arg += std::string(home_dir) + '/';
                    }
                    else {
                        arg += x + '/';
                    }

                }
                arg.pop_back();
                m[FILE_ARGS].clear();
                m[ARGS].push_back(arg);
            }

        }
        else if (tr->children[0]->non_terminal == NT_FLAG) {
            to_leaf(tr->children[0], m, NT_FLAG);
        }
        else {
            to_leaf(tr->children[0], m, ARGS);
        }
        return;
    }

    recursive_args(tr->children[0], m);
    if (tr->children.size() != 1) {
        recursive_args(tr->children[1], m);
    }
}

static tree_map_t tree2map(Tree<Symbol, std::string>* tr) {
    tree_map_t m;

    if (tr->children[1]->non_terminal == NT_VAR) {
        to_leaf(tr->children[1], m, NT_VAL);
        to_leaf(tr->children[2], m, NT_VALNULLABLE);
    }

    else if (tr->children[1]->non_terminal == NT_PRG) {
        recursive_file(tr->children[1], m, PROG);
    }

    if (tr->children[2]->non_terminal == NT_ARGS) {
        recursive_args(tr->children[2], m);
    }
    return m;
}


void handle_exec(ArgsMap<Symbol, std::vector<std::string>>& arg_map) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cout << "Fork failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        auto [arg_for_c, prg_name] = arg_map.map2vector();
        execvp(prg_name.c_str(), const_cast<char* const*>(arg_for_c.data()));
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &errno, 0);
        if (errno == 256) {
            std::cout << "myshell: command not found: " << arg_map.get_tree_map()[PROG][0] << std::endl;
        }
    }
}



#endif // SYNTAX_TREE_PARSER_H
