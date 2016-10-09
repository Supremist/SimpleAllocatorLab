cmake_minimum_required(VERSION 3.1)
project(lab1 CXX)

include_directories(common)

set(HEADER_LIST
    simpleallocator.h
    common/abstractallocator.h
    common/blockalignutils.h
    common/memoryrange.h
    common/memorypage.h
    )

set(SRC_LIST
    main.cpp
    simpleallocator.cpp
    common/abstractallocator.cpp
    common/blockalignutils.cpp
    common/memoryrange.cpp
    )

set(TEMPLATES_LIST
    common/memorypage.hpp
    )

add_executable(${PROJECT_NAME} ${HEADER_LIST} ${TEMPALTES_LIST} ${SRC_LIST})
target_compile_features(lab1 PRIVATE cxx_range_for)
