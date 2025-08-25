#include <iostream>
#include <fstream>

#include "Parser.h"

int main() {
	auto gamestate = parse_savefile("autosave_2301.02.01.sav");
	std::cout << "Country[0] economic power " << gamestate.root_obj["country"]["0"]["economy_power"].number() << '\n';
	std::ofstream out("out.json");
	if (out.is_open()) {
		out << gamestate.root_obj.to_json();
		out.close();
	}
	return 0;
}