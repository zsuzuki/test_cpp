cmake_minimum_required(VERSION 3.0)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include_directories(/usr/local/include inc)
link_directories(/usr/local/lib)

find_package(SQLite CONFIG)
find_package(lz4 CONFIG)

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
##
project(dynamiccast)
set(dcast_src dynamic_cast.cpp)
add_executable(dynamiccast ${dcast_src})
##
project(charcount)
set(ccount_src charcount.cpp)
add_executable(charcount ${ccount_src})
##
project(makerandom)
set(mrand_src make_random.cpp)
add_executable(makerandom ${mrand_src})
##
project(zstd_test)
set(zstd_src zstd_test.cpp)
add_executable(zstd_test ${zstd_src})
target_link_libraries(zstd_test zstd)
##
project(sqlite_test)
set(sqlite_src sqlite3.cpp)
add_executable(sqlite_test ${sqlite_src})
target_link_libraries(sqlite_test sqlite3)
##
project(lz4_test)
set(lz4_src lz4.cpp)
add_executable(lz4_test ${lz4_src})
target_link_libraries(lz4_test lz4)
##
project(game)
add_custom_command (
  OUTPUT
    ${PROJECT_SOURCE_DIR}/game_struct.hpp
  COMMAND structbuilder
    -hpp ${PROJECT_SOURCE_DIR}/game_struct.hpp
    ${PROJECT_SOURCE_DIR}/struct.toml
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/struct.toml)
set(game_src game_test.cpp)
add_executable(game ${game_src})
add_custom_target(generate_gamestruct DEPENDS ${PROJECT_SOURCE_DIR}/game_struct.hpp)
add_dependencies(game generate_gamestruct)
target_link_libraries(game pthread)
##
project(md5_test)
set(md5_src md5.cpp)
add_executable(md5_test ${md5_src})
