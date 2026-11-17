#ifndef EXP_C_INTERFACE
#define EXP_C_INTERFACE

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" {
	EXPORT char* parse_text_sysfile_to_json(const char* c_path);
	EXPORT char* parse_text_savefile_to_json(const char* c_path);
	EXPORT char* parse_zip_savefile_to_json(const char* c_path);
	EXPORT void delete_c_str_ptr(const char* ptr);
}

#endif
