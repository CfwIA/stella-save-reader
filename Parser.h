#include <chrono>
#include <variant>
#include <list>
#include <map>
#include <string>

#include "PoolAlloc.h"

#ifndef PARSER
#define PARSER

struct st_obj {
	enum type {
		UNDEFINED,
		STRING,
		LIST,
		OBJECT,
		NUMBER,
		INTEGER
	} datatype = UNDEFINED;
	//using field_t = std::map<std::string_view, st_obj>;
	//using vector_t = std::list<st_obj>;
	using field_t = std::map<std::string_view, st_obj, std::less<std::string_view>, PoolAllocator<std::pair<const std::string_view, st_obj>>>;
	using vector_t = std::list<st_obj, PoolAllocator<st_obj>>;
	std::variant<std::string_view, vector_t, field_t, double, long long> data;

	void to_json_private(std::ostringstream& oss) const;
	std::string to_json() const;
	operator std::string_view& ();
	operator double();
	std::string str() const;
	vector_t& list();
	const vector_t& list() const;
	double number() const;
	field_t& obj();
	const field_t& obj() const;
	st_obj& operator[](const size_t index);
	const st_obj& operator[](const size_t index) const;
	st_obj& operator[](const char* const fieldname);
	const st_obj& operator[](const char* const fieldname) const;
	bool has_field(const std::string& f) const;
	std::string get_datatype_str() const;
};

struct st_gamestate {
	std::string original_data;
	st_obj root_obj;
	std::string meta_data;
	st_obj meta_obj;
	std::chrono::system_clock::duration parse_time;
	std::chrono::system_clock::duration load_time;
};

st_gamestate parse_savefile(const std::string& path);

#endif
