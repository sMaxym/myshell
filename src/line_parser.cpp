#include "../include/line_parser.h"

ArgsMap<Symbol, std::vector<std::string>> parse_line(std::string& line) {

    init_clean(line);
    auto syntax = ll1_parser(line.c_str());
    if (syntax->children.size() == 1) {
        throw std::runtime_error("bad syntax");
    }


    ArgsMap<Symbol, std::vector<std::string>> arg_map(tree2map(syntax->children[0]));
    return arg_map;
}

std::vector<ArgsMap<Symbol, std::vector<std::string>>>
line2argsmap(std::string& line, const std::string& cwd) {
    std::vector<std::string> line_vector;
    boost::split( line_vector, line.substr(0, line.size() - line.find('#')), boost::is_any_of("|"));
    std::vector<ArgsMap<Symbol, std::vector<std::string>>> args_maps;
    for (auto& st: line_vector) {
        ArgsMap args_map = parse_line(st);
        args_map.get_tree_map()[CURRENT_PATH].push_back(cwd);
        args_maps.push_back(args_map);

    }
    return args_maps;
}
