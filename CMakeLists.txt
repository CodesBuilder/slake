cmake_minimum_required(VERSION 3.1)
project(hake VERSION 0.1.0)

set(C_STANDARD 99)

add_definitions(-D _CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

file(GLOB_RECURSE HAKE_SRC ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HAKE_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE COMMON_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

BISON_TARGET(hake src/hake.y ${PROJECT_BINARY_DIR}/hake.tab.c)
FLEX_TARGET(hake src/hake.l ${PROJECT_BINARY_DIR}/hake.l.c)
ADD_FLEX_BISON_DEPENDENCY(hake hake)

add_executable(hake ${BISON_hake_OUTPUTS} ${FLEX_hake_OUTPUTS} ${HAKE_SRC} ${HAKE_HEADERS} ${COMMON_HEADERS})
