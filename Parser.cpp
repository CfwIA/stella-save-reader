#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <string_view>
#include <optional>
#include <functional>
#include <filesystem>
#include <future>
#include <stack>
#include <deque>

#include <unarr.h>

#include "Parser.h"

using namespace std::chrono;

std::vector<Pool> memory;

bool is_printable_char(char c) {
	return c >= 32 && c < 127;
}

std::unordered_map<st_obj::type, std::string> string_type_table {
	{st_obj::type::INTEGER, "integer"},
	{st_obj::type::LIST, "list"},
	{st_obj::type::NUMBER, "number"},
	{st_obj::type::OBJECT, "object"},
	{st_obj::type::STRING, "string"},
	{st_obj::type::UNDEFINED, "undefined"}
};

std::string purify_str(const std::string_view& v) {
	std::string str = std::string(v);
	for (auto& a : str) {
		if (!is_printable_char(a))
			a = ' ';
	}
	return str;
}

void st_obj::to_json_private(std::ostringstream &oss) const
{
	if (datatype == NUMBER)
	{
		oss << std::get<3>(data);
		return;
	}
	if (datatype == INTEGER)
	{
		oss << std::get<4>(data);
		return;
	}
	if (datatype == STRING)
	{
		oss << '\"' << purify_str(std::get<0>(data)) << '\"';
		return;
	}
	if (datatype == LIST)
	{
		oss << "[";
		const auto &list = std::get<1>(data);
		for (auto it = list.begin(); it != list.end(); ++it)
		{
			if (it != list.begin())
				oss << ',';
			it->to_json_private(oss);
		}
		oss << ']';
		return;
	}
	if (datatype == OBJECT)
	{
		const auto &fields = std::get<2>(data);
		if (fields.size() == 0)
		{
			oss << "{}";
			return;
		}
		oss << '{';
		for (auto it = fields.begin(); it != fields.end(); ++it)
		{
			if (it != fields.begin())
				oss << ',';
			oss << '\"' << it->first << "\":";
			it->second.to_json_private(oss);
		}
		oss << '}';
		return;
	}
	oss << "null";
}

std::string st_obj::to_json() const
{
	std::ostringstream oss;
	to_json_private(oss);
	return oss.str();
}

st_obj::operator std::string_view &()
{
	if (datatype != STRING)
		throw std::runtime_error("std::string_view conversion operator error");
	return std::get<0>(data);
}

st_obj::operator double()
{
	if (datatype != NUMBER)
		throw std::runtime_error("operator double conversion operator error");
	return std::get<3>(data);
}

std::string st_obj::str() const
{
	if (datatype != STRING)
		throw std::runtime_error("str() function error");
	return std::string(std::get<0>(data));
}

st_obj::vector_t &st_obj::list()
{
	if (datatype != LIST)
		throw std::runtime_error("str() function error");
	return std::get<1>(data);
}

const st_obj::vector_t &st_obj::list() const
{
	if (datatype != LIST)
		throw std::runtime_error("str() function error");
	return std::get<1>(data);
}

double st_obj::number() const
{
	if (datatype != NUMBER)
		throw std::runtime_error("operator double conversion operator error");
	return std::get<3>(data);
}

st_obj::field_t &st_obj::obj()
{
	if (datatype != OBJECT)
		throw std::runtime_error("obj() function error");
	return std::get<2>(data);
}

const st_obj::field_t &st_obj::obj() const
{
	if (datatype != OBJECT)
		throw std::runtime_error("obj() function error");
	return std::get<2>(data);
}

st_obj &st_obj::operator[](const size_t index)
{
	if (datatype != LIST)
		throw std::runtime_error("list subscription operator error");
	auto it = std::get<1>(data).begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return *it;
}

const st_obj &st_obj::operator[](const size_t index) const
{
	if (datatype != LIST)
		throw std::runtime_error("list subscription operator error");
	auto it = std::get<1>(data).begin();
	for (size_t i = 0; i < index; ++i)
		++it;
	return *it;
}

st_obj &st_obj::operator[](const char *const fieldname)
{
	if (datatype != OBJECT)
		throw std::runtime_error("object subscription operator error");
	return std::get<2>(data).at(fieldname);
}

const st_obj &st_obj::operator[](const char *const fieldname) const
{
	if (datatype != OBJECT)
		throw std::runtime_error("object subscription operator error");
	return std::get<2>(data).at(fieldname);
}

bool st_obj::has_field(const std::string &f) const
{
	return datatype == OBJECT && std::get<2>(data).find(f) != std::get<2>(data).end();
}

std::string st_obj::get_datatype_str() const
{
	switch (datatype)
	{
	case st_obj::type::INTEGER:
		return "int";
	case st_obj::type::LIST:
		return "list";
	case st_obj::type::NUMBER:
		return "double";
	case st_obj::type::OBJECT:
		return "object";
	case st_obj::type::STRING:
		return "string";
	default:
		return "undefined";
	}
}

bool is_separator(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool is_field_id_char(char c) {
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-' || c == '\"';
}

std::optional<long long> parse_to_int(const std::string_view& v) {
	if (v.size() == 0) return std::nullopt;
	if (v[0] == '-') {
		auto db = parse_to_int(v.substr(1));
		return db.has_value() ? db.value() * -1 : db;
	}
	for(const auto& a : v) {
		if (a < '0' || a > '9') return std::nullopt;
	}
	if (v.size() > 18) return std::nullopt;
	long long value = 0LL;
	for(const auto& va : v) {
		value *= 10;
		value += va - '0';
	}
	return value;
}

std::optional<double> parse_to_double(const std::string_view& v) {
	if (v.size() == 0)
		return std::nullopt;
	if (v[0] == '-') {
		auto db = parse_to_double(v.substr(1));
		if (db.has_value())
			return -1.0 * db.value();
		return db;
	}
	double value = 0.0;
	size_t i = 0;
	while (v[i] >= '0' && v[i] <= '9') {
		value *= 10.0;
		value += v[i] - '0';
		++i;
		if (i >= v.size()) return value;
	}

	if (v[i] != '.')
		return std::nullopt;
	++i;
	if (i >= v.size())
		return value;
	double multiplier = 1.0;
	size_t cpt = 0;
	while (v[i] >= '0' && v[i] <= '9') {
		multiplier /= 10.0;
		value += double(v[i] - '0') * multiplier;
		++i;
		if (i >= v.size() || cpt >= 10) return value;
	}
	return std::nullopt;
}

// If fields, it's determined by the id of the field immediatly followed by "=" (no spaces between field id and =). Field name respect regex [a-zA-Z_0-9]*
// If no fields, it's a list, type of list is determined by first non space / tab / newline char encountered : if it's a {, it's an object, if it's a ", it's a string (seems non quoted array of strings don't exist), if it's a number it's a int array
// Separator is the space and newline, they both act as separators, there's no " =" existing in the file
st_obj::type get_object_type(const std::string_view& text, size_t position) {
	while (is_separator(text[position]))
		++position;

	if (text[position] == '{')
		return st_obj::type::LIST;
	
	// Step 2 : parse field name
	if (text[position] == '"') {
		++position;
		while (text[position] != '"')
			++position;
		++position;
	}
	else {
		while (is_field_id_char(text[position]))
			++position;
	}

	while (is_separator(text[position])) ++position;
	return text[position] == '=' ? st_obj::type::OBJECT : st_obj::type::LIST;
}

struct {
	int obj, str, list;
} parse_call_count;

st_obj parse_string_number(const std::string_view& text, size_t& position) {
	++parse_call_count.str;
	if (text[position] == '\"') {
		size_t start = position;
		do {
			++position;
		} while (text[position] != '\"');
		
		st_obj obj;
		obj.datatype = st_obj::STRING;
		obj.data = text.substr(start + 1, position - 1 - start);
		++position;
		return obj;
	}

	// Read of text until next line separator
	size_t start = position;
	while (!is_separator(text[position])) {
		++position;
	}

	st_obj value;
	auto data = text.substr(start, position - start);
	auto intval = parse_to_int(data);
	if (intval.has_value()) {
		value.datatype = st_obj::type::INTEGER;
		value.data = intval.value();
		return value;
	}
	auto doubleval = parse_to_double(data);
	if (doubleval.has_value()) {
		value.datatype = st_obj::type::NUMBER;
		value.data = doubleval.value();
		return value;
	}

	value.datatype = st_obj::type::STRING;
	value.data = data;
	if (data == "paragon_memory_vault")
		throw std::runtime_error("ERR");
	return value;
}

st_obj parse_object(const std::string_view& text, size_t& position);

st_obj parse_list(const std::string_view& text, size_t& position) {
	++parse_call_count.list;
	st_obj value;
	value.datatype = st_obj::LIST;
	value.data = st_obj::vector_t{};

	for (;;) {
		while (is_separator(text[position]))
			++position;

		// No field id, directly either an object / list, a string or an int
		if (text[position] == '{') {
			++position;
			auto objt = get_object_type(text, position);
			if (objt == st_obj::type::UNDEFINED)
				throw std::runtime_error("Error while parsing object : can't determine if next datatype is either list or object");
			if (objt == st_obj::type::OBJECT)
				std::get<1>(value.data).push_back(parse_object(text, position));
			if (objt == st_obj::type::LIST)
				std::get<1>(value.data).push_back(parse_list(text, position));
		}
		else {
			std::get<1>(value.data).push_back(parse_string_number(text, position));
		}

		// Go over blank spaces
		while (is_separator(text[position])) {
			++position;
		}

		if (text[position] == '}') {
			++position;
			return value;
		}
	}
}

st_obj parse_object(const std::string_view& text, size_t& position) {
	++parse_call_count.obj;
	st_obj value;
	value.datatype = st_obj::OBJECT;
	value.data = st_obj::field_t{};

	for (;;) {
		// Go over blank spaces
		while (is_separator(text[position])) {
			++position;
			if (position >= text.size()) {
				return value;
			}
		}

		if (text[position] == '}') {
			++position;
			return value;
		}

		// Parse field id, if quote is at the start, then skip it
		std::string_view fieldname;
		if (text[position] == '"') {
			++position;
			size_t fieldidstart = position;
			while (text[position] != '"')
				++position;
			fieldname = text.substr(fieldidstart, position - fieldidstart - 1);
			++position;
		}
		else {
			size_t field_id_start = position;
			while (is_field_id_char(text[position]))
				++position;
			fieldname = text.substr(field_id_start, position - field_id_start);
		}

		while (is_separator(text[position]))
			++position;

		// Is '='
		if (text[position] != '=' && text[position] != '{')
			throw std::runtime_error("Error while parsing object : non existing assignement character");

		if (text[position] == '=') ++position;

		// Check what's next, either object, int, string or list
		while (is_separator(text[position]))
			++position;
		if (text[position] == '{') { // Can be either object or list
			++position;
			// First, check if the object isn't empty
			while (is_separator(text[position])) ++position;
			if (text[position] == '}') {
				std::get<2>(value.data)[fieldname] = st_obj{};
				++position;
				continue;
			}
			auto objt = get_object_type(text, position);
			if (objt == st_obj::type::UNDEFINED)
				throw std::runtime_error("Error while parsing object : can't determine if next datatype is either list or object");
			if (std::get<2>(value.data).contains(fieldname)) {
				if (std::get<2>(value.data).at(fieldname).datatype != st_obj::LIST) {
					st_obj nob;
					nob.datatype = st_obj::type::LIST;
					auto vtor = st_obj::vector_t{};
					vtor.push_back(std::get<2>(value.data).at(fieldname));
					nob.data = std::move(vtor);
					std::get<2>(value.data).at(fieldname) = std::move(nob);
				}
				std::get<1>(std::get<2>(value.data).at(fieldname).data).push_back(
					objt == st_obj::type::OBJECT ? (parse_object(text, position)) : (parse_list(text, position))
				);
			} else {
				if (objt == st_obj::type::OBJECT)
					std::get<2>(value.data)[fieldname] = (parse_object(text, position));
				if (objt == st_obj::type::LIST)
					std::get<2>(value.data)[fieldname] = (parse_list(text, position));
			}
		}
		else {
			if (std::get<2>(value.data).contains(fieldname)) {
				if (std::get<2>(value.data).at(fieldname).datatype != st_obj::LIST) {
					// Transform to list
					st_obj nob;
					nob.datatype = st_obj::LIST;
					auto vtor = st_obj::vector_t{};
					vtor.push_back(std::get<2>(value.data).at(fieldname));
					nob.data = std::move(vtor);
					std::get<2>(value.data).at(fieldname) = std::move(nob);
				}
				std::get<1>(std::get<2>(value.data).at(fieldname).data).push_back(parse_string_number(text, position));
			} else {
				std::get<2>(value.data)[fieldname] = (parse_string_number(text, position));
			}
		}

		if (position >= text.size())
			return value;

		// Go over blank spaces
		while (is_separator(text[position])) {
			++position;
			if (position >= text.size()) {
				return value;
			}
		}
		
		if (text[position] == '}') {
			++position;
			return value;
		}
	}
}

st_gamestate parse_savefile(const std::string& path) {
	auto unarrstart = system_clock::now();
	auto* strm = ar_open_file(path.c_str());
	if (strm == nullptr)
		throw std::runtime_error("Can't open save file");

	auto* arc = ar_open_zip_archive(strm, false);
	if (arc == nullptr)
		throw std::runtime_error("Can't open zip archive");

	st_gamestate state;
	while (ar_parse_entry(arc)) {
		std::string entryname = ar_entry_get_name(arc);
		if (entryname == "gamestate") {
			size_t size = ar_entry_get_size(arc);
			std::vector<char> bytes(size);
			ar_entry_uncompress(arc, bytes.data(), size);

			state.original_data = std::string(bytes.data(), size);
			state.load_time = system_clock::now() - unarrstart;

			size_t position = 0;
			auto start = system_clock::now();
			if (state.original_data.starts_with("EU4txt"))
				position = 7;
			state.root_obj = parse_object(state.original_data, position);
			auto stop = system_clock::now();
			state.parse_time = stop - start;
		}
		if (entryname == "meta") {
			size_t size = ar_entry_get_size(arc);
			std::vector<char> bytes(size);
			ar_entry_uncompress(arc, bytes.data(), size);

			state.meta_data = std::string(bytes.data(), size);

			size_t position = 0;
			if (state.meta_data.starts_with("EU4txt"))
				position = 7;
			state.meta_obj = parse_object(state.meta_data, position);
		}
	}
	ar_close_archive(arc);
	ar_close(strm);
	return state;
}

std::string loadup_zip_gamestate(const std::string& path) {
	auto* strm = ar_open_file(path.c_str());
	if (strm == nullptr)
		throw std::runtime_error("Can't open save file");

	auto* arc = ar_open_zip_archive(strm, false);
	if (arc == nullptr)
		throw std::runtime_error("Can't open zip archive");

	while (ar_parse_entry(arc)) {
		std::string entryname = ar_entry_get_name(arc);
		if (entryname == "gamestate") {
			size_t size = ar_entry_get_size(arc);
			std::vector<char> bytes(size);
			ar_entry_uncompress(arc, bytes.data(), size);
			ar_close_archive(arc);
			ar_close(strm);
			return std::string(bytes.data(), size);
		}
	}
	throw std::runtime_error("Can't file gamestate entry in save file");
}

template<typename T>
std::vector<T> populate_dataset(const std::string& folder, std::function<T(const st_gamestate&)> func) {
	auto start = system_clock::now();
	std::vector<T> rtval;
	using namespace std::filesystem;
	size_t filecount = 0;
	for (const auto& dir_entry : recursive_directory_iterator(path(folder)))
		++filecount;
	size_t progress = 0;

	for (const auto& dir_entry : recursive_directory_iterator(path(folder))) {
		++progress;
		int sizeofbar = (progress * 65 / filecount);
		std::cout << "\rparsing " << dir_entry.path().filename() << "  |" << std::string(sizeofbar, '#') << std::string(65 - sizeofbar, ' ') << '|';
		rtval.push_back(func(parse_savefile(dir_entry.path().string())));
		auto drt = duration_cast<milliseconds>(system_clock::now() - start);
		std::cout << "  " << (drt / progress).count() << "ms ";
		if (drt.count() > 9999)
			std::cout << duration_cast<seconds>(drt).count() << "s   ";
		else
			std::cout << drt.count() << "ms";

	}
	std::cout << "\nFull parsing time taken : " << duration_cast<milliseconds>(system_clock::now()-start).count() << "ms" << '\n';
	return rtval;
}

void populate_dataset(const std::string& folder, std::function<void(const st_gamestate&)> func) {
	using namespace std::filesystem;
	size_t filecount = 0;
	for (const auto& dir_entry : recursive_directory_iterator(path(folder)))
		++filecount;
	size_t progress = 0;

	auto start = std::chrono::system_clock::now();
	for (const auto& dir_entry : recursive_directory_iterator(path(folder))) {
		++progress;
		int sizeofbar = (progress * 75 / filecount);
		std::cout << "\rparsing " << dir_entry.path().filename() << "  |" << std::string(sizeofbar, '#') << std::string(75 - sizeofbar, ' ') << '|';
		func(parse_savefile(dir_entry.path().string()));
		std::cout << "  " << (duration_cast<milliseconds>(system_clock::now() - start) / progress).count() << "ms";
	}
}

