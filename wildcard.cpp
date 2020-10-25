#include "wildcards.h"

std::vector<std::string> wildcard_handler(const std::string& name, const char * pattern)
{
    std::vector<std::string> f_names;
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        int n = fnmatch(pattern, dp->d_name, FNM_EXTMATCH);
        if (n == 0) {
            f_names.push_back(dp->d_name);
        }
    }
    closedir(dirp);
    return f_names;
}
