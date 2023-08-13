from ctypes import CDLL, c_int, c_char_p, c_void_p

lib = CDLL('./touchscreen4py.so')

lib.createTouchScreen.argtypes = [c_int]
lib.createTouchScreen.restype = c_void_p

lib.testStr.restype = c_char_p
lib.testStr.argtypes = [c_void_p]

ts_p: c_void_p = lib.createTouchScreen(1)
byte_arr: bytes = lib.testStr(ts_p)

print(f"======Return type: {type(byte_arr)}, value: {byte_arr}")
if byte_arr:
    print(f"======Decoded: {byte_arr.decode(errors='replace')}")
