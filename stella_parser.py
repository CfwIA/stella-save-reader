import ctypes
import os

from pathlib import Path

_libpath = os.path.abspath("para-savefile-parselib.dll")
_dll = ctypes.CDLL(_libpath)


def _get_dll_function(fname: str, ret_type, *args_type):
    funct = getattr(_dll, fname)
    funct.restype = ret_type
    funct.argtypes = args_type
    return funct

    
_parse_text_sysfile = _get_dll_function("parse_text_sysfile_to_json", ctypes.c_void_p, ctypes.c_char_p)
_parse_text_savefile = _get_dll_function("parse_text_savefile_to_json", ctypes.c_void_p, ctypes.c_char_p)
_parse_zip_savefile = _get_dll_function("parse_zip_savefile_to_json", ctypes.c_void_p, ctypes.c_char_p)
_delete_c_str_ptr = _get_dll_function("delete_c_str_ptr", None, ctypes.c_void_p)


def parse_system_file(path: Path) -> str | None:
    if not path.exists():
        raise ValueError("Path must exist")
    if not path.is_file():
        raise ValueError("Path must be file")
    utfpath = str(path).encode(encoding="utf-8")
    rawptr = _parse_text_sysfile(utfpath)
    if not rawptr:
        raise RuntimeError("Null pointer returned")
    try:
        strptr = ctypes.c_char_p(rawptr)
        result = strptr.value.decode("utf-8")
        return result
    finally:
        _delete_c_str_ptr(rawptr)

def parse_text_savefile(path: Path) -> str | None:
    if not path.exists():
        raise ValueError("Path must exist")
    if not path.is_file():
        raise ValueError("Path must be file")
    utfpath = str(path).encode(encoding="utf-8")
    rawptr = _parse_text_savefile(utfpath)
    if not rawptr:
        raise RuntimeError("Null pointer returned")
    try:
        strptr = ctypes.c_char_p(rawptr)
        result = strptr.value.decode("utf-8")
        return result
    finally:
        _delete_c_str_ptr(rawptr)


def parse_zip_savefile(path: Path) -> str | None:
    if not path.exists():
        raise ValueError("Path must exist")
    if not path.is_file():
        raise ValueError("Path must be file")
    utfpath = str(path).encode(encoding="utf-8")
    rawptr = _parse_zip_savefile(utfpath)
    if not rawptr:
        raise RuntimeError("Null pointer returned")
    try:
        strptr = ctypes.c_char_p(rawptr)
        result = strptr.value.decode("utf-8")
        return result
    finally:
        _delete_c_str_ptr(rawptr)

