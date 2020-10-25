#ifndef ARGSMAP_H
#define ARGSMAP_H
#include "syntax_analysis.h"
#include <algorithm>
template <typename T, typename U>
class ArgsMap
{

    typedef std::map<T, U> tree_map_t;
    tree_map_t tree_map;
public:

    ArgsMap(const tree_map_t& tree_map) : tree_map(tree_map){};
    ~ArgsMap() = default;
    std::pair<std::vector<const char*>, std::string> map2vector();

    inline tree_map_t& get_tree_map() { return tree_map; };
};

template <typename T, typename U>
std::pair<std::vector<const char*>, std::string> ArgsMap<T, U>::map2vector()
{
       std::vector<const char*> args_for_c;
       std::string prog;
       std::string prog_name = tree_map[PROG][tree_map[PROG].size() - 1];
       size_t ind = prog_name.find('.');
       for (const auto& x: tree_map[PROG])
       {
           prog += x + '/';
       }
       prog.pop_back();
       if (ind != std::string::npos && prog_name.substr(ind) == ".msh")
       {
           tree_map[ARGS].insert(tree_map[ARGS].begin(), prog);
           prog = "myshell";
       }

       args_for_c.push_back(prog.c_str());

       for (const auto& x: tree_map[ARGS]) {
           args_for_c.push_back(x.c_str());
       }
       for (const auto& x: tree_map[NT_FLAG]) {
           args_for_c.push_back(x.c_str());
       }
       for (size_t i = 0; i < tree_map[KEY].size(); ++i) {
           prog = tree_map[KEY][i] + '=' + tree_map[VALUE][i];
           args_for_c.push_back(prog.c_str());
       }
       args_for_c.push_back(nullptr);
       return std::pair(args_for_c, args_for_c[0]);
   }


#endif // ARGSMAP_H
