cmake_minimum_required(VERSION 3.1)
project(lab1 CXX)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_compile_features(lab1 PRIVATE cxx_range_for)
