cmake_minimum_required(VERSION 3.14.0)
project(para-savefile-parser VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 23)
add_executable(para-savefile-parser source.cpp Parser.cpp )

if(MSVC)
	message(STATUS "Using Microsoft Visual C++ compiler")
	# the unarchive library (unarr) needs location to be indicated for MSVC windows compiling. Probably need to extend to all compilation on windows platform

	include(FetchContent)

	FetchContent_Declare(
		unarr
		GIT_REPOSITORY https://github.com/selmf/unarr.git
		GIT_TAG        master
	)
	FetchContent_MakeAvailable(unarr)
endif()


target_link_libraries(para-savefile-parser unarr)
