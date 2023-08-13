import ctypes

testlib = ctypes.CDLL('./touchscreen4py.so')
testlib.testStr.restype = ctypes.c_char_p
byte_arr:bytes = testlib.testStr()
print(f"======Return type: {type(byte_arr)}, value: {byte_arr}, decoded: {byte_arr.decode(errors='replace')}")
