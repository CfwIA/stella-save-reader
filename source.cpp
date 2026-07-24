#include <iostream>
#include <fstream>
#include <chrono>

#include "Parser.h"

std::string construct_svname(const std::string& gamename, const std::string& date) {
	std::string str = gamename + '_' + date;
	for(auto& c : str) {
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
			c = '_';
		}
	}
	return str + ".json";
}

int main(int argc, char** argv) {

	std::vector<std::string> args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	if (args.size() < 2) {
		std::cout << "Not enough CLI args";
		return 1;
	}

	std::string readfilepath = args.at(1);
	std::string outdir = args.size() == 2 ? "" : args.at(2) ;

	if (readfilepath.ends_with(".hoi4")) {
		auto gamestate = parse_raw_hoi4_savefile(readfilepath);
		auto outfile = construct_svname(
			gamestate.root_obj["player"].str(),
			gamestate.root_obj["date"].str()
		);
		std::ofstream out(outdir + outfile);
		std::cout << "Load time : " << std::chrono::duration_cast<std::chrono::milliseconds>(gamestate.load_time);
		std::cout << "\nParse time : " << std::chrono::duration_cast<std::chrono::milliseconds>(gamestate.parse_time);
		if (out.is_open()) {
			out << gamestate.root_obj.to_json();
			out.close();
		}
		return 0;
	}

	if (readfilepath.ends_with(".eu4")) {
		auto gamestate = parse_raw_eu4_savefile(readfilepath);
		auto outfile = construct_svname(
			gamestate.root_obj["player"].str(),
			gamestate.root_obj["date"].str()
		);
		std::ofstream out(outdir + outfile);
		std::cout << "Load time : " << std::chrono::duration_cast<std::chrono::milliseconds>(gamestate.load_time);
		std::cout << "\nParse time : " << std::chrono::duration_cast<std::chrono::milliseconds>(gamestate.parse_time);
		if (out.is_open()) {
			out << gamestate.root_obj.to_json();
			out.close();
		}
		return 0;
	}

	auto gamestate = parse_stellaris_savefile(readfilepath);
	auto outfile = construct_svname(
		gamestate.meta_obj.obj().contains("name") ? gamestate.meta_obj["name"].str() : gamestate.meta_obj["displayed_country_name"].str(),
		gamestate.meta_obj["date"].str());
	std::ofstream out(outdir + outfile);
	if (out.is_open()) {
		out << gamestate.root_obj.to_json();
		out.close();
	}
	return 0;
}