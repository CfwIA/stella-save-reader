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


_read_raw_file = _get_dll_function("read_raw_file", ctypes.c_void_p, ctypes.c_char_p)
_read_zipped_file = _get_dll_function("read_zipped_file", ctypes.c_void_p, ctypes.c_char_p)
_delete_c_str_ptr = _get_dll_function("delete_c_str_ptr", None, ctypes.c_void_p)

def read_raw_file(path: Path) -> str | None:
    if not path.exists():
        raise ValueError("Path must exist")
    if not path.is_file():
        raise ValueError("Path must be file")
    utfpath = str(path).encode(encoding="utf-8")
    rawptr = _read_raw_file(utfpath)
    if not rawptr:
        raise RuntimeError("Null pointer returned")
    try:
        strptr = ctypes.c_char_p(rawptr)
        result = strptr.value.decode("utf-8")
        return result
    finally:
        _delete_c_str_ptr(rawptr)


def read_zipped_file(path: Path) -> str | None:
    if not path.exists():
        raise ValueError("Path must exist")
    if not path.is_file():
        raise ValueError("Path must be file")
    utfpath = str(path).encode(encoding="utf-8")
    rawptr = _read_zipped_file(utfpath)
    if not rawptr:
        raise RuntimeError("Null pointer returned")
    try:
        strptr = ctypes.c_char_p(rawptr)
        result = strptr.value.decode("utf-8")
        return result
    finally:
        _delete_c_str_ptr(rawptr)

