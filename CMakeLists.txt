cmake_minimum_required(VERSION 3.14.0)
project(para-savefile-parser VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE pybind11_DIR
)
find_package(pybind11 CONFIG REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(para-savefile-parser "source.cpp" "parser.cpp")
add_library(para-savefile-parselib SHARED "parser.cpp" "c_interface.cpp")

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


target_link_libraries(para-savefile-parser PRIVATE unarr)
target_link_libraries(para-savefile-parselib PRIVATE unarr)

pybind11_add_module(paraducks_parser "pybindlib.cpp" "parser.cpp")
target_link_libraries(paraducks_parser PRIVATE unarr)

