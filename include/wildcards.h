#ifndef WILDCARDS_H
#define WILDCARDS_H

#include <vector>
#include <dirent.h>
#include <fnmatch.h>
#include <string>

std::vector<std::string> wildcard_handler(const std::string& name, const char * pattern);

#endif // WILDCARDS_H
