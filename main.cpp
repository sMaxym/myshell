#include <iostream>

#include <filesystem>
#include <vector>
#include <sstream>
#include "inter_functions.h"
#include <iterator>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/algorithm/string.hpp>
#include "pipes.h"
#include "execution.h"

namespace cout_vector {
    template<class T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& values)
    {
        stream << "[ ";
        copy( begin(values), end(values), std::ostream_iterator<T>(stream, " ") );
        stream << ']';
        return stream;
    }
}
using namespace cout_vector;

int add_pwd2path(const std::string& cwd) {
    auto path = getenv("PATH");
    if (path == nullptr) {
        return 1;
    }
    std::string str_path(path);
    size_t ind = cwd.find('/');
    if (ind == std::string::npos) {
        return 0;
    }
    str_path += ':' + std::filesystem::absolute(cwd.substr(0, ind)).string();
    return setenv("PATH", str_path.c_str(), 1);

}

int main(int argc, char* argv[])
{
    if (add_pwd2path(std::string(argv[0])) != 0)
    {
        std::cerr << "Failed to add PATH" << std::endl;
        return -1;
    }
    if (argc > 1) {
        std::string prg_name = argv[1];
        return run_script(prg_name);
    }
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

        buf = readline(ss.str().c_str());
        std::string line = "ls -9 1> fil1 >&2";
        if (buf == nullptr) {
            std::cerr << "Readline error" << std::endl;
            _exit(EXIT_FAILURE);
        }
        line = buf;
        free(buf);
        if (line.empty())
            continue;
        add_history(line.c_str());
        std::vector<std::string> line_vector;
        boost::split( line_vector, line.substr(0, line.size() - line.find('#')), boost::is_any_of("|"));
        std::vector<ArgsMap<Symbol, std::vector<std::string>>> args_maps;
        for (auto& st: line_vector) {
            ArgsMap args_map = parse_line(st, cwd);
            args_map.get_tree_map()[CURRENT_PATH].push_back(cwd);
            args_maps.push_back(args_map);

        }

        if (args_maps.size() == 1) {
            if (kernel_command(args_maps[0].get_tree_map()) < 0) {
                handle_exec(args_maps[0]);
            }
        } else {
            pipe_handler(args_maps);
        }


    }
    return 0;
}
