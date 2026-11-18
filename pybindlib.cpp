#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <chrono>
#include <filesystem>

#include "parser.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace py = pybind11;

py::dict convert_obj_to_dict(const st_obj& obj);

py::list convert_obj_to_list(const st_obj& obj) {
	py::list result;
	const auto& llist = obj.list();

	for (const auto& elem : llist) {

		switch (elem.datatype) {
		case st_obj::type::UNDEFINED:
			break;
		case st_obj::type::INTEGER:
			result.append(elem.integer());
			break;
		case st_obj::type::NUMBER:
			result.append(elem.number());
			break;
		case st_obj::type::STRING:
			result.append(elem.str());
			break;
		case st_obj::type::LIST:
			result.append(convert_obj_to_list(elem));
			break;
		case st_obj::type::OBJECT:
			result.append(convert_obj_to_dict(elem));
			break;
		}
	}
	return result;
}

py::dict convert_obj_to_dict(const st_obj& obj) {
	py::dict result;
	const auto& lobj = obj.obj();

	for (const auto& fd : lobj) {

		auto key = py::str(fd.first);

		switch (fd.second.datatype) {
		case st_obj::type::UNDEFINED:
			break;
		case st_obj::type::INTEGER:
			result[key] = fd.second.integer();
			break;
		case st_obj::type::NUMBER:
			result[key] = fd.second.number();
			break;
		case st_obj::type::STRING:
			result[key] = fd.second.str();
			break;
		case st_obj::type::LIST:
			result[key] = convert_obj_to_list(fd.second);
			break;
		case st_obj::type::OBJECT:
			result[key] = convert_obj_to_dict(fd.second);
			break;
		}
	}

	return result;
}


py::dict pybind_parse_text_sysfile(const std::filesystem::path& filepath) {
	if (!std::filesystem::is_regular_file(filepath)) {
		PyErr_SetString(PyExc_FileNotFoundError, "File path was not found");
		throw py::error_already_set();
	}
	auto state = parse_text_sysfile(filepath.string());

	auto t1 = system_clock::now();
	py::dict parse_result = convert_obj_to_dict(state.root_obj);
	auto t2 = system_clock::now();

	py::dict perfodata;

	perfodata["load_time"] = duration_cast<milliseconds>(state.load_time).count();
	perfodata["parse_time"] = duration_cast<milliseconds>(state.parse_time).count();
	perfodata["transform_time"] = duration_cast<milliseconds>(t2 - t1).count();

	py::dict result;
	result["main_object"] = parse_result;
	result["performance"] = perfodata;

	return result;
}

py::dict pybind_parse_text_savefile(const std::filesystem::path& filepath) {
	if (!std::filesystem::is_regular_file(filepath)) {
		PyErr_SetString(PyExc_FileNotFoundError, "File path was not found");
		throw py::error_already_set();
	}
	auto state = parse_text_savefile(filepath.string());

	auto t1 = system_clock::now();
	py::dict parse_result = convert_obj_to_dict(state.root_obj);
	auto t2 = system_clock::now();

	py::dict perfodata;

	perfodata["load_time"] = duration_cast<milliseconds>(state.load_time).count();
	perfodata["parse_time"] = duration_cast<milliseconds>(state.parse_time).count();
	perfodata["transform_time"] = duration_cast<milliseconds>(t2 - t1).count();

	py::dict result;
	result["main_object"] = parse_result;
	result["performance"] = perfodata;

	return result;
}

py::dict pybind_parse_zip_savefile(const std::filesystem::path& filepath) {
	if (!std::filesystem::is_regular_file(filepath)) {
		PyErr_SetString(PyExc_FileNotFoundError, "File path was not found");
		throw py::error_already_set();
	}
	auto state = parse_zipped_savefile(filepath.string());

	auto t1 = system_clock::now();
	py::dict parse_result = convert_obj_to_dict(state.root_obj);
	auto t2 = system_clock::now();

	py::dict perfodata;

	perfodata["load_time"] = duration_cast<milliseconds>(state.load_time).count();
	perfodata["parse_time"] = duration_cast<milliseconds>(state.parse_time).count();
	perfodata["transform_time"] = duration_cast<milliseconds>(t2 - t1).count();

	py::dict result;
	result["main_object"] = parse_result;
	result["performance"] = perfodata;

	return result;
}

PYBIND11_MODULE(paraducks_parser, m) {
	m.doc() = "Paradox Interactive based games save (and modding/system) file parser";

	m.def("parse_text_sysfile", pybind_parse_text_sysfile, py::arg("filepath"),
		"Returns a dictionary representing the state of the parsed savefile in the \"main_object\" object");

	m.def("parse_text_savefile", pybind_parse_text_savefile, py::arg("filepath"),
		"Parses a text save file, and returns a dictionary representing the state of the parsed savefile in the \"main_object\" object. Supports parsing files starting with HOI4txt and EU4txt");

	m.def("parse_zip_savefile", pybind_parse_zip_savefile, py::arg("filepath"),
		"Parses a zipped file (like stellaris' .sav save files), and returns a dictionary representing the state of the parsed savefile in the \"main_object\" object");
}
