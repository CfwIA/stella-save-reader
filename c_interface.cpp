#include "c_interface.h"

#include <string>
#include <filesystem>

#include "parser.h"

extern "C" {


	char* read_raw_file(const char* c_path)
	{
		auto path = std::string(c_path);
		if (!std::filesystem::is_regular_file(path)) {
			std::string error_msg = "ERROR: Path is not regular file";
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
		size_t fsize = std::filesystem::file_size(path);
		try {
			auto state = parse_raw_file(path);
			auto jsonstr = state.root_obj.to_json(fsize);
			char* rawcstr = new char[jsonstr.length() + 1];
			std::strcpy(rawcstr, jsonstr.c_str());
			return rawcstr;
		}
		catch (const std::exception& e) {
			std::string error_msg = "C++ Exception: " + std::string(e.what());
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
		catch (...) {
			std::string error_msg = "C++ Exception: Unknown error";
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
	}

	char* read_zipped_file(const char* c_path)
	{
		auto path = std::string(c_path);
		if (!std::filesystem::is_regular_file(path)) {
			std::string error_msg = "ERROR: Path is not regular file";
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
		size_t fsize = std::filesystem::file_size(path);
		try {
			auto state = parse_zipped_file(path);
			auto jsonstr = state.root_obj.to_json(fsize);
			char* rawcstr = new char[jsonstr.length() + 1];
			std::strcpy(rawcstr, jsonstr.c_str());
			return rawcstr;
		}
		catch (const std::exception& e) {
			std::string error_msg = "C++ Exception: " + std::string(e.what());
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
		catch (...) {
			std::string error_msg = "C++ Exception: Unknown error";
			char* output = new char[error_msg.length() + 1];
			std::strcpy(output, error_msg.c_str());
			return output;
		}
	}

	void delete_c_str_ptr(const char* ptr)
	{
		delete[] ptr;
	}

}
