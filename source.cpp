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

	auto start = std::chrono::system_clock::now();
	auto gamestate = parse_savefile(readfilepath);
	auto timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count();
	std::cout << "Parsing took " << timems << "ms\n";
	std::ofstream out(outfile);
	if (out.is_open()) {
		auto jsontimestart = std::chrono::system_clock::now();
		out << gamestate.root_obj.to_json();
		auto timems2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count();
		std::cout << "Printing to json took " << timems2 << "ms\n";
		out.close();
	}
	return 0;
}