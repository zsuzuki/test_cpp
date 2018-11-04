cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")

##
project(string)
set(string_src string_encode.cpp)
add_executable(string ${string_src})
##
project(number)
set(number_src number_shuffle.cpp)
add_executable(number ${number_src})
##
project(zlibtest)
set(zlib_src zlib_test.cpp)
add_executable(zlibtest ${zlib_src})
target_link_libraries(zlibtest z)
##
project(toml)
set(toml_src toml.cpp)
add_executable(toml ${toml_src})
##
project(functioncall)
set(funccall_src function_call.cpp)
add_executable(functioncall ${funccall_src})
##
project(memcached)
set(memcached_src memcached.cpp)
add_executable(memcached ${memcached_src})
