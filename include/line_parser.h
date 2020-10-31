#ifndef LINE_PARSER_H
#define LINE_PARSER_H
#include "argsmap.h"
#include "syntax_tree_parser.h"
#include <boost/algorithm/string.hpp>

ArgsMap<Symbol, std::vector<std::string>> parse_line(std::string& line);

std::vector<ArgsMap<Symbol, std::vector<std::string>>>
line2argsmap(std::string& line, const std::string& cwd);

#endif // LINE_PARSER_H
