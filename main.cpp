#include <iostream>

#include <filesystem>
#include <vector>
#include <sstream>
#include "inter_functions.h"
#include <iterator>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;


template<class T>
std::ostream& operator<<(ostream& stream, const std::vector<T>& values)
{
    stream << "[ ";
    copy( begin(values), end(values), ostream_iterator<T>(stream, " ") );
    stream << ']';
    return stream;
}

int add_pwd2path(std::string&& cwd) {
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
        std::cout << "Failed to add PATH" << std::endl;
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
        std::string line = "cat ..";
        buf = readline(ss.str().c_str());

        if (buf == nullptr) {
            std::cout << "Readline error" << std::endl;
            _exit(EXIT_FAILURE);
        }
        line = buf;
        free(buf);
        if (line.empty())
            continue;
        add_history(line.c_str());
        parse_line(line, cwd);

    }
    return 0;
}
