#ifndef EXP_C_INTERFACE
#define EXP_C_INTERFACE

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" {
	EXPORT char* read_raw_file(const char* c_path);
	EXPORT char* read_zipped_file(const char* c_path);
	EXPORT void delete_c_str_ptr(const char* ptr);
}

#endif
