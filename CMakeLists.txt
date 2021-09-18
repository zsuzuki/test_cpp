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
find_package(leveldb CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.66.0 REQUIRED COMPONENTS thread iostreams filesystem exception)
find_package(PNG 1.6.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})

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
project(temptest)
set(temptest_src temptest.cpp)
add_executable(temptest ${temptest_src})
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
project(lvdb_test)
set(lvdb_src leveldb.cpp)
add_executable(lvdb_test ${lvdb_src})
target_link_libraries(lvdb_test PRIVATE leveldb::leveldb)
##
project(boostfs_test)
set(boostfs_src boostfs.cpp)
add_executable(boostfs_test ${boostfs_src})
target_link_libraries(boostfs_test PRIVATE ${Boost_LIBRARIES})
##
project(conv_arimg)
set(conv_arimg_src conv_arimg.cpp)
add_executable(conv_arimg ${conv_arimg_src})
target_link_libraries(conv_arimg PRIVATE ${PNG_LIBRARIES})
##
project(arduino_test)
set(arduino_test_src arduino_test.cpp)
add_executable(arduino_test ${arduino_test_src})
##
project(uuid)
set(uuid_src uuid.cpp)
add_executable(uuid ${uuid_src})
target_link_libraries(uuid PRIVATE ${Boost_LIBRARIES})
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
##
project(bdfread)
set(bdfread_src bdfread.cpp)
add_executable(bdfread ${bdfread_src})
