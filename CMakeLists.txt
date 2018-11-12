cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
if(VERSION VERSION_GREATER 3.0)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
else()
    set(CMAKE_CXX_FLAGS "-std=c++14")
endif()
include_directories(/usr/local/include inc)
link_directories(/usr/local/lib)

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
##
project(worker)
set(worker_src thread_test.cpp)
add_executable(worker ${worker_src})
target_link_libraries(worker pthread)
##
project(queue)
set(queue_src queue.cpp)
add_executable(queue ${queue_src})
