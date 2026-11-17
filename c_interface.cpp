#include "c_interface.h"

#include <string>
#include <filesystem>

#include "parser.h"

char* create_return_message(const std::string& str) {
	char* output = new char[str.length() + 1];
	std::strcpy(output, str.c_str());
	return output;
}

extern "C" {

	char* parse_text_sysfile_to_json(const char* c_path)
	{
		auto path = std::string(c_path);
		if (!std::filesystem::is_regular_file(path)) {
			return create_return_message("ERROR: Path is not regular file");
		}
		size_t fsize = std::filesystem::file_size(path);
		try {
			auto state = parse_text_sysfile(path);
			return create_return_message(state.root_obj.to_json(fsize));
		}
		catch (const std::exception& e) {
			return create_return_message("C++ Exception: " + std::string(e.what()));
		}
		catch (...) {
			return create_return_message("C++ Exception: Unknown error");
		}
	}

	char* parse_text_savefile_to_json(const char* c_path)
	{
		auto path = std::string(c_path);
		if (!std::filesystem::is_regular_file(path)) {
			return create_return_message("ERROR: Path is not regular file");
		}
		size_t fsize = std::filesystem::file_size(path);
		try {
			auto state = parse_text_savefile(path);
			return create_return_message(state.root_obj.to_json(fsize));
		}
		catch (const std::exception& e) {
			return create_return_message("C++ Exception: " + std::string(e.what()));
		}
		catch (...) {
			return create_return_message("C++ Exception: Unknown error");
		}
	}

	char* parse_zip_savefile_to_json(const char* c_path)
	{
		auto path = std::string(c_path);
		if (!std::filesystem::is_regular_file(path)) {
			return create_return_message("ERROR: Path is not regular file");
		}
		size_t fsize = std::filesystem::file_size(path);
		try {
			auto state = parse_zipped_savefile(path);
			return create_return_message(state.root_obj.to_json(fsize));
		}
		catch (const std::exception& e) {
			return create_return_message("C++ Exception: " + std::string(e.what()));
		}
		catch (...) {
			return create_return_message("C++ Exception: Unknown error");
		}
	}

	void delete_c_str_ptr(const char* ptr)
	{
		delete[] ptr;
	}

}
