#include "../include/syntax_tree_parser.h"

static void to_leaf(tree_t* tree, tree_map_t& trm, Symbol symbol,
             size_t children_index) {
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
                std::string mask;
                for (size_t i = 0; i < m[FILE_ARGS].size(); ++i) {
                    if (i == 0 && m[FILE_ARGS][i] == "~") {
                        auto home_dir = getenv("HOME");
                        arg += std::string(home_dir) + '/';
                    }
//                    else if (i == m[FILE_ARGS].size() - 1) {
//                        mask = m[FILE_ARGS][i];
//                    }
                    else {
                        arg += m[FILE_ARGS][i] + '/';

                    }

                }
//                if (arg.empty()) {
//                    arg = ".";
//                }
//                else {
                    arg.pop_back();
//                }

                m[FILE_ARGS].clear();
                m[ARGS].push_back(arg);
//                auto files = wildcard_handler(arg, mask.c_str());
//                for (const auto& file: files) {
//                    std::cout << file << std::endl;
//                    m[ARGS].push_back(arg + '/' + file);
//                }

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

static void recursive_rdcts(Tree<Symbol, std::string>* tr,
                            std::map<Symbol, std::vector<std::string>>& m) {
    if (tr->children[0]->is_leaf()) {
        return;
    }

    to_leaf(tr->children[0]->children[0], m, T_P_FROM);
    to_leaf(tr->children[0]->children[1], m, T_RDCT);
    to_leaf(tr->children[0]->children[2], m, T_P_TO);
    recursive_rdcts(tr->children[1], m);
}

tree_map_t tree2map(Tree<Symbol, std::string>* tr) {
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

    if (tr->children[3]->non_terminal == NT_RDCTS) {
        recursive_rdcts(tr->children[3], m);
    }

    if (tr->children[4]->non_terminal == T_BG_PRC) {
        to_leaf(tr->children[4], m, T_BG_PRC);
    }
    return m;
}
