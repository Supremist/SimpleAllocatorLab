cmake_minimum_required(VERSION 3.1)
project(lab2 CXX)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
