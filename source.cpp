#include <iostream>
#include <fstream>
#include <chrono>

#include "Parser.h"

int main(int argc, char** argv) {

	std::vector<std::string> args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	std::string readfilepath = args[1];
	std::string outfile = args[2];

	auto gamestate = parse_savefile(readfilepath);
	std::cout << gamestate.meta_obj["date"].str() << '\n';
	std::ofstream out(outfile);
	if (out.is_open()) {
		out << gamestate.root_obj.to_json();
		out.close();
	}
	return 0;
}