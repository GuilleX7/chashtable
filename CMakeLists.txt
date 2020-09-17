cmake_minimum_required(VERSION 2.8)
project(chashtable-test LANGUAGES C)

add_library(chashtable STATIC "src/hash_table.c")
add_executable(chashtable-test "test/test.c")
target_link_libraries(chashtable-test "chashtable")