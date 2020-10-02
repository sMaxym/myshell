#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <filesystem>
#include <vector>
#include <sstream>
#include "inter_functions.h"
#include <iterator>
#include <readline/history.h>
#include "argsmap.h"
#include <readline/readline.h>
#include <readline/history.h>
typedef std::map<Symbol, std::vector<std::string>> tree_map_t;
typedef Tree<Symbol, std::string> tree_t;
using namespace std;


template<class T>
std::ostream& operator<<(ostream& stream, const std::vector<T>& values)
{
    stream << "[ ";
    copy( begin(values), end(values), ostream_iterator<T>(stream, " ") );
    stream << ']';
    return stream;
}

void f(ArgsMap<Symbol, std::vector<std::string>>& arg_map) {
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
    }
}

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
    if (tr->children.size() != 1) {
        recursive_file(tr->children[0], m, symbol);
        recursive_file(tr->children[2], m, symbol);
    }
    else {
        to_leaf(tr, m, symbol);
    }

}


static void recursive_args(Tree<Symbol, std::string>* tr,
                    std::map<Symbol, std::vector<std::string>>& m) {
    if (tr->is_leaf()) {
        return;
    }
    if (tr->non_terminal == NT_ARG) {
        if (tr->children[0]->non_terminal == NT_KEY) {
            to_leaf(tr->children[0], m, KEY);
            to_leaf(tr->children[2], m, VALUE);
        }
        else if (tr->children[0]->non_terminal == NT_VAL)  {
            if (tr->children[0]->children[0]->non_terminal == NT_VARCALL) {
                auto new_arg = getenv(
                            tr->children[0]->children[0]->
                        children[1]->children[0]->terminal.c_str()
                        );
                if (new_arg != nullptr) {
                    m[ARGS].push_back(std::string(new_arg));
                }
            }

            else {
                recursive_file(tr->children[0]->children[0], m, FILE_ARGS);
                std::string arg;
                for (auto& x: m[FILE_ARGS]) {
                    arg += x + '/';
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

int main(int argc, char* argv[])
{
    char* buf;
    while (true) {
        std::stringstream ss;
        auto cwd = std::filesystem::current_path().string();
        if (errno == 0) {
            ss << "\033[1;32m" <<  errno;
        } else {
            ss << "\033[1;31m" <<  errno;
        }

        ss << " " << "\033[0;33m" << cwd;
        ss << "\033[1;33m" << " $ " << "\033[1;39m";
        std::string line = "cat ..";
        buf = readline(ss.str().c_str());

        if (buf == nullptr) {
            std::cout << "Readline error" << std::endl;
            _exit(EXIT_FAILURE);
        }
        line = buf;
        if (line.empty())
            continue;
        add_history(buf);
        free(buf);
        init_clean(line);
        auto syntax = ll1_parser(line.c_str());
        ArgsMap args_map(tree2map(syntax->children[0]));
        args_map.get_tree_map()[CURRENT_PATH].push_back(cwd);
        for (auto& x: args_map.get_tree_map()) {
            std::cout << x.first << " " << x.second << std::endl;
        }
        if (kernel_command(args_map.get_tree_map()) < 0) {
            f(args_map);
        }

    }
    return 0;
}
