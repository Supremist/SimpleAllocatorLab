cmake_minimum_required(VERSION 3.1)
project(lab2 CXX)

set(COMMON_DIR ../lab1/common)

include_directories(${COMMON_DIR})

set(HEADER_LIST
	classifiedallocator.h
	blockmemoryrange.h
	pageinfo.h
	${COMMON_DIR}/abstractallocator.h
	${COMMON_DIR}/blockalignutils.h
	${COMMON_DIR}/memoryrange.h
	${COMMON_DIR}/memorymanager.h
	${COMMON_DIR}/allocatedblock.h
	)

set(SRC_LIST
	main.cpp
	classifiedallocator.cpp
	blockmemoryrange.cpp
	${COMMON_DIR}/abstractallocator.cpp
	${COMMON_DIR}/blockalignutils.cpp
	${COMMON_DIR}/memoryrange.cpp
	${COMMON_DIR}/allocatedblock.cpp
	)

set(TEMPLATES_LIST
	${COMMON_DIR}/memorymanager.hpp
	)

add_executable(${PROJECT_NAME} ${HEADER_LIST} ${TEMPALTES_LIST} ${SRC_LIST})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
