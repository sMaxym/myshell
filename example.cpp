#include <iostream>
#include <string>
#include "syntax_analysis.h"

int main(int argc, char** argv) {
	std::string str = "~/prg ~";
	init_clean(str);
	auto synt = ll1_parser(str.c_str());

	std::cout << " " << std::endl;

  	return 0;
}
